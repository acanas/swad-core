#!/usr/bin/env python3
"""
swad_dev.py - Local development workflow for SWAD (Shared Workspace At a Distance).

Runs the whole platform in user space (no root, no docker, no Apache):

  * MariaDB server with its datadir inside local-dev/root/db
  * SWAD CGI binaries (built with `make LOCAL_DEV=1`) served by a small
    Python CGI server on http://localhost:8000
  * A fictional university (institution/center/degree), a fictional
    professor and fictional students enrolled in a fictional course

Usage:
  ./swad_dev.py setup     # one-time: deps, build, directories, database, seed
  ./swad_dev.py start     # start MariaDB + web server (background)
  ./swad_dev.py stop      # stop both
  ./swad_dev.py status    # show what is running
  ./swad_dev.py seed      # (re)create the fictional professor/students/course
  ./swad_dev.py reset     # stop and delete ALL local dev data (root/ except deps)
  ./swad_dev.py serve     # run only the web server in foreground (for debugging)

State lives in local-dev/root/ (git-ignored). Configuration used at runtime:
  SWAD_DEV_ROOT  override root dir (default: <repo>/local-dev/root)
  SWAD_DEV_PORT  override port     (default: 8000, must match build flag)
"""

import base64
import hashlib
import json
import os
import secrets
import shutil
import signal
import socket
import subprocess
import sys
import time
import urllib.parse
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path

# --------------------------------------------------------------------------
# Paths and constants
# --------------------------------------------------------------------------

HERE = Path(__file__).resolve().parent          # local-dev/
REPO = HERE.parent                              # swad-core/
ROOT = Path(os.environ.get("SWAD_DEV_ROOT", HERE / "root")).resolve()
PORT = int(os.environ.get("SWAD_DEV_PORT", "8000"))

CGI_BIN = ROOT / "cgi-bin"
WWW = ROOT / "www"                    # document root; assets in www/swad
PRIVATE = ROOT / "private"
DB_DIR = ROOT / "db"
LOGS = ROOT / "logs"
DEPS = ROOT / "deps" / "gsoap"
STATE_FILE = ROOT / "state.json"
SOCK = DB_DIR / "mysql.sock"
PID_HTTP = ROOT / "httpd.pid"
PID_DB = ROOT / "mysqld.pid"

LANGS = ["ca", "de", "en", "es", "fr", "gn", "it", "pl", "pt", "tr"]
GSOAP_PKG_URL = ("https://fastly.mirror.pkgbuild.com/extra/os/x86_64/"
                 "gsoap-2.8.145-1-x86_64.pkg.tar.zst")

# Fictional seed data
SEED = {
    "country":  {"code": 1, "alpha2": "ZZ", "name": "Testland"},
    "place":    {"code": 1, "name": "Test City"},
    "inst":     {"code": 1, "name": "Fictional University"},
    "center":   {"code": 1, "name": "North Campus"},
    "degtype":  {"code": 1, "name": "Degree"},
    "degree":   {"code": 1, "name": "Computer Science"},
    "dept":     {"code": 1, "name": "Computer Science Department"},
    "course":   {"code": 1, "inscrs": "TST0001",
                 "short": "SWAD 101", "full": "Introduction to the SWAD platform"},
    "users": [
        {"cod": 1000, "id": "prof1", "nick": "prof", "pw": "professor2026",
         "name": "Alejandro", "surn1": "Rega", "surn2": "Sanchez",
         "email": "dr.plasmadrid@fictional.edu", "role": 5, "admin": True},
        {"cod": 1001, "id": "student1", "nick": "student1", "pw": "student2026",
         "name": "Alice", "surn1": "Smith", "surn2": "",
         "email": "alice.smith@fictional.edu", "role": 3, "admin": False},
        {"cod": 1002, "id": "student2", "nick": "student2", "pw": "student2026",
         "name": "Bob", "surn1": "Jones", "surn2": "Brown",
         "email": "bob.jones@fictional.edu", "role": 3, "admin": False},
        {"cod": 1003, "id": "student3", "nick": "student3", "pw": "student2026",
         "name": "Carol", "surn1": "Davis", "surn2": "",
         "email": "carol.davis@fictional.edu", "role": 3, "admin": False},
        {"cod": 1004, "id": "student4", "nick": "student4", "pw": "student2026",
         "name": "Dan", "surn1": "Miller", "surn2": "",
         "email": "dan.miller@fictional.edu", "role": 3, "admin": False},
        {"cod": 1005, "id": "student5", "nick": "student5", "pw": "student2026",
         "name": "Eva", "surn1": "Wilson", "surn2": "Taylor",
         "email": "eva.wilson@fictional.edu", "role": 3, "admin": False},
    ],
}


# --------------------------------------------------------------------------
# Small helpers
# --------------------------------------------------------------------------

def log(msg):
    print(f"[swad-dev] {msg}", flush=True)


def die(msg, code=1):
    print(f"[swad-dev] ERROR: {msg}", file=sys.stderr, flush=True)
    sys.exit(code)


def run(cmd, **kw):
    log("+ " + " ".join(str(c) for c in cmd))
    return subprocess.run(cmd, check=True, **kw)


def load_state():
    if STATE_FILE.exists():
        return json.loads(STATE_FILE.read_text())
    return {}


def save_state(state):
    ROOT.mkdir(parents=True, exist_ok=True)
    STATE_FILE.write_text(json.dumps(state, indent=2))


def db_password():
    """Return (create if needed) the DB password stored in state."""
    state = load_state()
    pw = state.get("db_password")
    if not pw:
        pw = secrets.token_urlsafe(18)
        state["db_password"] = pw
        save_state(state)
    return pw


def swad_password_hash(plain):
    """SWAD stores SHA-512(password) as unpadded base64url (86 chars)."""
    return base64.urlsafe_b64encode(
        hashlib.sha512(plain.encode("utf-8")).digest()
    ).rstrip(b"=").decode("ascii")


def sql_str(s):
    return "'" + str(s).replace("\\", "\\\\").replace("'", "\\'") + "'"


# --------------------------------------------------------------------------
# Setup steps
# --------------------------------------------------------------------------

def check_tools():
    tools = ["gcc", "make", "python3", "mariadb-install-db", "mariadbd",
             "mariadb", "curl", "tar"]
    missing = [t for t in tools if shutil.which(t) is None]
    if missing:
        die(f"missing tools: {', '.join(missing)}")


def fetch_gsoap():
    soapcpp2 = DEPS / "usr/bin/soapcpp2"
    if soapcpp2.exists():
        return
    log("Downloading gSOAP package ...")
    DEPS.parent.mkdir(parents=True, exist_ok=True)
    pkg = DEPS.parent / "gsoap.pkg.tar.zst"
    run(["curl", "-fSL", "-o", str(pkg), GSOAP_PKG_URL])
    run(["tar", "-xf", str(pkg), "-C", str(DEPS.parent)])
    if not soapcpp2.exists():
        die("gSOAP extraction failed: soapcpp2 not found")
    log("gSOAP ready at " + str(DEPS))


def generate_soap_stubs():
    soap_dir = REPO / "soap"
    if (soap_dir / "soapC.c").exists() and (soap_dir / "soapH.h").exists():
        return
    log("Generating gSOAP stubs ...")
    run([str(DEPS / "usr/bin/soapcpp2"), "-c", "-S",
         "-I" + str(DEPS / "usr/include"), "swad_web_service.h"],
        cwd=soap_dir)


def build():
    log("Building SWAD binaries (make LOCAL_DEV=1) ...")
    env = dict(os.environ)
    env["LOCAL_DEV_PORT"] = str(PORT)
    # Build shared objects via one target (parallel-safe), then link each
    # language sequentially: the per-language .o files share names, so `all`
    # with -j races between languages.
    run(["make", "LOCAL_DEV=1", f"LOCAL_DEV_PORT={PORT}", "-j8", "swad_en"],
        cwd=REPO, env=env)
    for lang in LANGS:
        run(["make", "LOCAL_DEV=1", f"LOCAL_DEV_PORT={PORT}", f"swad_{lang}"],
            cwd=REPO, env=env)
    missing = [l for l in LANGS if not (REPO / f"swad_{l}").exists()]
    if missing:
        die(f"build failed for: {', '.join(missing)}")


def write_swad_cfg():
    CGI_BIN.mkdir(parents=True, exist_ok=True)
    cfg = CGI_BIN / "swad.cfg"
    cfg.write_text(
        f"DATABASE_PASSWORD\t{db_password()}\n"
        f"SMTP_PASSWORD\t\t{secrets.token_urlsafe(12)}\n")
    cfg.chmod(0o600)


def populate_dirs():
    # --- CGI directory: binaries + helper commands ---
    CGI_BIN.mkdir(parents=True, exist_ok=True)
    for lang in LANGS:
        shutil.copy2(REPO / f"swad_{lang}", CGI_BIN / f"swad_{lang}")
    for src, dst in [
        (REPO / "chat/chat.html", CGI_BIN / "chat.html"),
        (REPO / "foto_promedio/foto_mediana", CGI_BIN / "foto_mediana"),
        (REPO / "foto_promedio/foto_promedio", CGI_BIN / "foto_promedio"),
        (REPO / "fotomaton/example/cascade.xml", CGI_BIN / "cascade.xml"),
        (REPO / "py/swad_smtp.py", CGI_BIN / "swad_smtp.py"),
        (REPO / "html/foot.html", CGI_BIN / "foot.html"),
        (REPO / "soap/swad.wsdl", CGI_BIN / "swad.wsdl"),
    ]:
        if src.exists():
            shutil.copy2(src, dst)
        else:
            log(f"warning: missing {src}")

    # --- Public web dir: root/www/swad ---
    pub = WWW / "swad"
    pub.mkdir(parents=True, exist_ok=True)

    def link(src, dst):
        dst = pub / dst
        if dst.is_symlink() or dst.exists():
            return
        dst.symlink_to(src)

    for d in ["icon", "js", "css", "jstz", "dropzone", "fontawesome", "chat"]:
        link(REPO / d, d)
    link(REPO / "js/mathjax-config.js", "mathjax-config.js")
    # Main JS/CSS bundles live at the root of /swad/ (see swad_copy.sh)
    for pat in ["swad*.js"]:
        for f in (REPO / "js").glob(pat):
            link(f, f.name)
    for pat in ["swad*.css"]:
        for f in (REPO / "css").glob(pat):
            link(f, f.name)
    for z in ["ins.zip", "ctr.zip", "deg.zip"]:
        src = REPO / "html/swad" / z
        if src.exists():
            link(src, z)

    # Generated asset dirs expected by SWAD
    for d in ["ins", "ctr", "deg", "crs", "grp", "prj", "photo", "rep",
              "banner", "med", "tmp", "processing", "links", "logo"]:
        (pub / d).mkdir(exist_ok=True)

    # API/WSDL endpoints
    for d in ["api", "ws"]:
        (pub / d).mkdir(exist_ok=True)
        wsdl = pub / d / "swad.wsdl"
        if not wsdl.exists() and (REPO / "soap/swad.wsdl").exists():
            wsdl.symlink_to(REPO / "soap/swad.wsdl")

    # Data protection page (must exist per config comments)
    dp = pub / "data_protection"
    dp.mkdir(exist_ok=True)
    dp_index = REPO / "html/data_protection/index.html"
    if dp_index.exists() and not (dp / "index.html").exists():
        shutil.copy2(dp_index, dp / "index.html")

    # --- Rest of document root: symlink repo html/* (except swad/) ---
    for item in (REPO / "html").iterdir():
        if item.name == "swad":
            continue
        dst = WWW / item.name
        if not dst.exists() and not dst.is_symlink():
            dst.symlink_to(item)

    # --- Private dir ---
    for d in ["ins", "ctr", "deg", "crs", "grp", "prj", "usr", "out", "param",
              "photo", "test", "tmp", "zip", "media", "mark", "links",
              "logo", "processing"]:
        (PRIVATE / d).mkdir(parents=True, exist_ok=True)


# --------------------------------------------------------------------------
# Database
# --------------------------------------------------------------------------

def mariadb(args, **kw):
    return run(["mariadb", f"--socket={SOCK}", "-u", "root", *args], **kw)


def db_is_running():
    if not SOCK.exists():
        return False
    r = subprocess.run(
        ["mariadb-admin", f"--socket={SOCK}", "-u", "root", "ping"],
        capture_output=True)
    return r.returncode == 0


def db_start():
    if db_is_running():
        return
    if not (DB_DIR / "mysql").exists():
        log("Initializing MariaDB datadir ...")
        DB_DIR.mkdir(parents=True, exist_ok=True)
        LOGS.mkdir(parents=True, exist_ok=True)
        run(["mariadb-install-db", f"--datadir={DB_DIR}",
             "--auth-root-authentication-method=normal",
             "--skip-test-db"])
    log("Starting mariadbd ...")
    logf = open(LOGS / "mysqld.log", "ab")
    proc = subprocess.Popen(
        ["mariadbd",
         f"--datadir={DB_DIR}",
         f"--socket={SOCK}",
         f"--pid-file={PID_DB}",
         "--port=3306",
         "--bind-address=127.0.0.1",
         "--log-error=" + str(LOGS / "mariadb.err.log"),
         "--skip-log-bin"],
        stdout=logf, stderr=subprocess.STDOUT,
        start_new_session=True)
    for _ in range(120):
        if db_is_running():
            log(f"MariaDB up (pid {proc.pid})")
            return
        if proc.poll() is not None:
            die("mariadbd exited; see " + str(LOGS / "mariadb.err.log"))
        time.sleep(0.5)
    die("MariaDB did not start in time")


def db_stop():
    if db_is_running():
        subprocess.run(["mariadb-admin", f"--socket={SOCK}", "-u", "root",
                        "shutdown"], capture_output=True)
        for _ in range(60):
            if not db_is_running():
                break
            time.sleep(0.5)
    if PID_DB.exists():
        try:
            os.kill(int(PID_DB.read_text().strip()), signal.SIGTERM)
        except (ValueError, ProcessLookupError):
            pass


def db_create_schema_and_user():
    pw = db_password()
    log("Creating database schema ...")
    with open(REPO / "sql/swad.sql", "rb") as f:
        subprocess.run(
            ["mariadb", f"--socket={SOCK}", "-u", "root"],
            stdin=f, check=True)
    log("Creating 'swad' database user ...")
    mariadb(["-e",
             f"CREATE USER IF NOT EXISTS 'swad'@'localhost' IDENTIFIED BY {sql_str(pw)};"
             f"CREATE USER IF NOT EXISTS 'swad'@'127.0.0.1' IDENTIFIED BY {sql_str(pw)};"
             f"CREATE USER IF NOT EXISTS 'swad'@'%' IDENTIFIED BY {sql_str(pw)};"
             "GRANT ALL PRIVILEGES ON swad.* TO 'swad'@'localhost';"
             "GRANT ALL PRIVILEGES ON swad.* TO 'swad'@'127.0.0.1';"
             "GRANT ALL PRIVILEGES ON swad.* TO 'swad'@'%';"
             "FLUSH PRIVILEGES;"])


# --------------------------------------------------------------------------
# Seed: fictional university + professor + students + course
# --------------------------------------------------------------------------

def seed():
    db_start()
    s = SEED
    c, p, i, ct, d = (s["country"], s["place"], s["inst"],
                      s["center"], s["degree"])
    stmts = []

    # Wipe previous seed rows (fixed codes -> idempotent)
    user_cods = ",".join(str(u["cod"]) for u in s["users"])
    stmts += [
        f"DELETE FROM crs_users WHERE CrsCod={s['course']['code']};",
        f"DELETE FROM crs_courses WHERE CrsCod={s['course']['code']};",
        f"DELETE FROM usr_admins WHERE UsrCod IN ({user_cods});",
        f"DELETE FROM usr_ids WHERE UsrCod IN ({user_cods});",
        f"DELETE FROM usr_nicknames WHERE UsrCod IN ({user_cods});",
        f"DELETE FROM usr_emails WHERE UsrCod IN ({user_cods});",
        f"DELETE FROM usr_data WHERE UsrCod IN ({user_cods});",
        f"DELETE FROM deg_degrees WHERE DegCod={d['code']};",
        f"DELETE FROM deg_types WHERE DegTypCod={s['degtype']['code']};",
        f"DELETE FROM dpt_departments WHERE DptCod={s['dept']['code']};",
        f"DELETE FROM ctr_centers WHERE CtrCod={ct['code']};",
        f"DELETE FROM plc_places WHERE PlcCod={p['code']};",
        f"DELETE FROM ins_instits WHERE InsCod={i['code']};",
        f"DELETE FROM cty_countrs WHERE CtyCod={c['code']};",
    ]

    names = {"ca": "Testland", "de": "Testland", "en": "Testland",
             "es": "Testland", "fr": "Testland", "gn": "Testland",
             "it": "Testland", "pl": "Testland", "pt": "Testland",
             "tr": "Testland"}
    wwws = {k: "" for k in names}
    stmts.append(
        "INSERT INTO cty_countrs (CtyCod,Alpha2,MapAttribution,"
        + ",".join(f"Name_{k}" for k in names) + ","
        + ",".join(f"WWW_{k}" for k in names) + ") VALUES ("
        + f"{c['code']},{sql_str(c['alpha2'])},''"
        + "".join("," + sql_str(v) for v in names.values())
        + "".join("," + sql_str(v) for v in wwws.values()) + ");")

    stmts.append(
        "INSERT INTO ins_instits (InsCod,CtyCod,Status,RequesterUsrCod,"
        "ShortName,FullName,WWW) VALUES ("
        f"{i['code']},{c['code']},0,-1,"
        f"{sql_str(i['name'])},{sql_str(i['name'] + ' (fictional)')},'');")

    stmts.append(
        "INSERT INTO plc_places (PlcCod,InsCod,ShortName,FullName) VALUES ("
        f"{p['code']},{i['code']},{sql_str(p['name'])},{sql_str(p['name'])});")

    stmts.append(
        "INSERT INTO ctr_centers (CtrCod,InsCod,PlcCod,Status,RequesterUsrCod,"
        "Latitude,Longitude,Altitude,ShortName,FullName,WWW,PhotoAttribution)"
        " VALUES ("
        f"{ct['code']},{i['code']},{p['code']},0,-1,0,0,0,"
        f"{sql_str(ct['name'])},{sql_str(ct['name'] + ' (fictional)')},'','');")

    stmts.append(
        "INSERT INTO deg_types (DegTypCod,DegTypName) VALUES ("
        f"{s['degtype']['code']},{sql_str(s['degtype']['name'])});")

    stmts.append(
        "INSERT INTO deg_degrees (DegCod,CtrCod,DegTypCod,Status,"
        "RequesterUsrCod,ShortName,FullName,WWW) VALUES ("
        f"{d['code']},{ct['code']},{s['degtype']['code']},0,-1,"
        f"{sql_str(d['name'])},{sql_str(d['name'] + ' (fictional)')},'');")

    stmts.append(
        "INSERT INTO dpt_departments (DptCod,InsCod,ShortName,FullName,WWW)"
        " VALUES ("
        f"{s['dept']['code']},{i['code']},"
        f"{sql_str(s['dept']['name'])},{sql_str(s['dept']['name'])},'');")

    for u in s["users"]:
        stmts.append(
            "INSERT INTO usr_data (UsrCod,EncryptedUsrCod,Password,"
            "Surname1,Surname2,FirstName,Sex,Language,"
            "CtyCod,InsCod,DptCod,CtrCod,Comments) VALUES ("
            f"{u['cod']},{sql_str(secrets.token_urlsafe(32))},"
            f"{sql_str(swad_password_hash(u['pw']))},"
            f"{sql_str(u['surn1'])},{sql_str(u['surn2'])},"
            f"{sql_str(u['name'])},'unknown','en',"
            f"{c['code']},{i['code']},{s['dept']['code']},{ct['code']},'');")
        stmts.append(
            "INSERT INTO usr_ids (UsrCod,UsrID,CreatTime,Confirmed) VALUES ("
            f"{u['cod']},{sql_str(u['id'].upper())},NOW(),'Y');")
        stmts.append(
            "INSERT INTO usr_nicknames (UsrCod,Nickname,CreatTime) VALUES ("
            f"{u['cod']},{sql_str(u['nick'])},NOW());")
        stmts.append(
            "INSERT INTO usr_emails (UsrCod,E_mail,CreatTime,Confirmed)"
            " VALUES ("
            f"{u['cod']},{sql_str(u['email'])},NOW(),'Y');")
        stmts.append(
            "INSERT INTO crs_users (CrsCod,UsrCod,Role,Accepted) VALUES ("
            f"{s['course']['code']},{u['cod']},{u['role']},'Y');")
        if u["admin"]:
            stmts.append(
                "INSERT INTO usr_admins (UsrCod,Scope,Cod) VALUES ("
                f"{u['cod']},'Sys',0);")

    stmts.append(
        "INSERT INTO crs_courses (CrsCod,DegCod,Year,InsCrsCod,Status,"
        "RequesterUsrCod,ShortName,FullName,NumIndicators) VALUES ("
        f"{s['course']['code']},{d['code']},1,{sql_str(s['course']['inscrs'])},0,"
        f"{s['users'][0]['cod']},"
        f"{sql_str(s['course']['short'])},{sql_str(s['course']['full'])},-1);")

    sql = "USE swad;\n" + "\n".join(stmts) + "\n"
    log("Seeding fictional data ...")
    subprocess.run(["mariadb", f"--socket={SOCK}", "-u", "root"],
                   input=sql.encode(), check=True)
    log("Seed complete:")
    for u in s["users"]:
        log(f"  {u['id']:9s} password: {u['pw']}"
            + ("  (superuser)" if u["admin"] else ""))
    log(f"  course: {s['course']['full']} (code {s['course']['code']})")


# --------------------------------------------------------------------------
# Web server (CGI)
# --------------------------------------------------------------------------

def cgi_env(handler, script_path, path_info):
    headers = handler.headers
    env = {
        "GATEWAY_INTERFACE": "CGI/1.1",
        "SERVER_SOFTWARE": "swad-dev/1.0",
        "SERVER_PROTOCOL": handler.request_version,
        "REQUEST_METHOD": handler.command,
        "SCRIPT_NAME": script_path,
        "PATH_INFO": path_info,
        "QUERY_STRING": getattr(handler, "_query_string", ""),
        "REQUEST_URI": handler.path,
        "REMOTE_ADDR": handler.client_address[0],
        "SERVER_NAME": "localhost",
        "SERVER_PORT": str(PORT),
        "SERVER_ADMIN": "dev@localhost",
        "DOCUMENT_ROOT": str(WWW),
    }
    if handler.command in ("POST", "PUT"):
        cl = headers.get("Content-Length")
        if cl:
            env["CONTENT_LENGTH"] = cl
        ct = headers.get("Content-Type")
        if ct:
            env["CONTENT_TYPE"] = ct
    for k, v in headers.items():
        key = "HTTP_" + k.upper().replace("-", "_")
        if key not in ("HTTP_CONTENT_TYPE", "HTTP_CONTENT_LENGTH"):
            env[key] = v
    return env


class SwadHandler(BaseHTTPRequestHandler):
    server_version = "swad-dev/1.0"
    protocol_version = "HTTP/1.1"

    def log_message(self, fmt, *args):
        sys.stderr.write("[http] " + fmt % args + "\n")

    # -- routing ---------------------------------------------------------

    def _split(self):
        parts = urllib.parse.urlsplit(self.path)
        segs = [s for s in parts.path.split("/") if s]
        query = parts.query
        # urlsplit keeps %XX escapes; QUERY_STRING must stay raw
        if "?" in self.path:
            query = self.path.split("?", 1)[1]
        return segs, query, parts.path

    def _is_cgi(self, segs):
        return segs and segs[0] in LANGS and len(segs[0]) == 2

    def do_GET(self):
        self._handle()

    def do_POST(self):
        self._handle()

    def do_HEAD(self):
        self._handle(head_only=True)

    def _handle(self, head_only=False):
        segs, query, raw_path = self._split()
        self._query_string = query

        if not segs:
            # / -> pick language like Apache rewrite does
            lang = "en"
            accept = self.headers.get("Accept-Language", "")
            for l in LANGS:
                if l in accept.lower():
                    lang = l
                    break
            self.send_response(302)
            self.send_header("Location", f"/{lang}")
            self.send_header("Content-Length", "0")
            self.end_headers()
            return

        if self._is_cgi(segs):
            self._run_cgi(segs, query, head_only)
            return

        self._serve_static(raw_path, head_only)

    # -- CGI -------------------------------------------------------------

    def _run_cgi(self, segs, query, head_only):
        lang = segs[0]
        script = CGI_BIN / f"swad_{lang}"
        path_info = "/" + "/".join(segs[1:]) if len(segs) > 1 else ""
        script_name = "/" + lang

        length = int(self.headers.get("Content-Length") or 0)
        body = self.rfile.read(length) if length else b""

        env = cgi_env(self, script_name, path_info)
        env["QUERY_STRING"] = query
        env["SCRIPT_FILENAME"] = str(script)

        try:
            proc = subprocess.run(
                [str(script)], input=body, capture_output=True,
                env=env, cwd=str(CGI_BIN), timeout=None)
        except Exception as e:
            self._cgi_error(500, f"CGI failed to start: {e}")
            return

        if proc.stderr:
            sys.stderr.write(proc.stderr.decode("utf-8", "replace"))

        out = proc.stdout
        # split headers/body
        for sep in (b"\r\n\r\n", b"\n\n"):
            idx = out.find(sep)
            if idx >= 0:
                head, cgi_body = out[:idx], out[idx + len(sep):]
                break
        else:
            head, cgi_body = out, b""

        status = 200
        headers = []
        for line in head.decode("latin-1").splitlines():
            if not line.strip():
                continue
            if line.lower().startswith("status:"):
                try:
                    status = int(line.split(":", 1)[1].strip().split()[0])
                except ValueError:
                    pass
            elif ":" in line:
                k, v = line.split(":", 1)
                headers.append((k.strip(), v.strip()))

        has_len = any(k.lower() == "content-length" for k, _ in headers)
        has_type = any(k.lower() == "content-type" for k, _ in headers)
        if not has_type:
            headers.append(("Content-Type", "text/html; charset=latin1"))
        if not has_len:
            headers.append(("Content-Length", str(len(cgi_body))))
        headers.append(("Connection", "close"))

        try:
            self.send_response(status)
            for k, v in headers:
                self.send_header(k, v)
            self.end_headers()
            if not head_only:
                self.wfile.write(cgi_body)
        except (BrokenPipeError, ConnectionResetError):
            pass

    def _cgi_error(self, code, msg):
        body = f"<h1>SWAD dev error</h1><p>{msg}</p>".encode()
        self.send_response(code)
        self.send_header("Content-Type", "text/html")
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    # -- static ----------------------------------------------------------

    def _serve_static(self, raw_path, head_only):
        path = urllib.parse.unquote(raw_path.split("?", 1)[0])
        # Note: do not resolve() — asset dirs are symlinks into the repo
        fs = os.path.normpath(str(WWW / path.lstrip("/")))
        if not (fs == str(WWW) or fs.startswith(str(WWW) + os.sep)):
            self.send_error(403)
            return
        fs = Path(fs)
        if fs.is_symlink():
            fs = fs.resolve()
        if fs.is_dir():
            fs = fs / "index.html"
        if not fs.is_file():
            self.send_error(404)
            return

        import mimetypes
        ctype = mimetypes.guess_type(str(fs))[0] or "application/octet-stream"
        if ctype == "text/javascript":
            ctype = "application/javascript"
        data = fs.read_bytes()
        self.send_response(200)
        self.send_header("Content-Type", ctype)
        self.send_header("Content-Length", str(len(data)))
        self.send_header("Last-Modified", self.date_time_string(fs.stat().st_mtime))
        self.end_headers()
        if not head_only:
            self.wfile.write(data)


def http_start():
    if PID_HTTP.exists():
        try:
            pid = int(PID_HTTP.read_text().strip())
            os.kill(pid, 0)
            log(f"Web server already running (pid {pid})")
            return
        except (ValueError, ProcessLookupError):
            pass
    LOGS.mkdir(parents=True, exist_ok=True)
    logf = open(LOGS / "http.log", "ab")
    # Rebuild env so child CGIs see a sane PATH etc.
    proc = subprocess.Popen(
        [sys.executable, str(Path(__file__).resolve()), "serve",
         "--port", str(PORT)],
        stdout=logf, stderr=subprocess.STDOUT,
        start_new_session=True)
    PID_HTTP.write_text(str(proc.pid))
    # wait until accepting connections
    for _ in range(100):
        try:
            with socket.create_connection(("127.0.0.1", PORT), 0.2):
                break
        except OSError:
            if proc.poll() is not None:
                die("web server exited; see " + str(LOGS / "http.log"))
            time.sleep(0.1)
    else:
        die("web server did not start in time")
    log(f"Web server up: http://localhost:{PORT}/  (pid {proc.pid})")


def http_stop():
    if PID_HTTP.exists():
        try:
            pid = int(PID_HTTP.read_text().strip())
            os.killpg(os.getpgid(pid), signal.SIGTERM)
        except (ValueError, ProcessLookupError, PermissionError):
            pass
        PID_HTTP.unlink(missing_ok=True)


def serve(port):
    handler = lambda *a, **k: SwadHandler(*a, **k, )
    httpd = ThreadingHTTPServer(("127.0.0.1", port), SwadHandler)
    httpd.daemon_threads = True
    log(f"Serving on http://localhost:{port}/ (Ctrl+C to stop)")
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        pass


# --------------------------------------------------------------------------
# CLI
# --------------------------------------------------------------------------

def cmd_setup():
    check_tools()
    ROOT.mkdir(parents=True, exist_ok=True)
    LOGS.mkdir(parents=True, exist_ok=True)
    fetch_gsoap()
    generate_soap_stubs()
    build()
    populate_dirs()
    write_swad_cfg()
    db_start()
    db_create_schema_and_user()
    seed()
    log("Setup complete. Run './swad_dev.py start' then open "
        f"http://localhost:{PORT}/")


def cmd_start():
    db_start()
    http_start()
    state = load_state()
    log("SWAD dev is running:")
    log(f"  URL:      http://localhost:{PORT}/")
    log(f"  Professor login: {SEED['users'][0]['id']} / {SEED['users'][0]['pw']}")
    log(f"  Student login:    {SEED['users'][1]['id']}..{SEED['users'][-1]['id']} / {SEED['users'][1]['pw']}")
    log(f"  Logs:      {LOGS}")
    log("  Stop with: ./swad_dev.py stop")


def cmd_stop():
    http_stop()
    db_stop()
    log("Stopped.")


def cmd_status():
    print(f"root:   {ROOT}")
    print(f"port:   {PORT}")
    print(f"db:     {'running' if db_is_running() else 'stopped'}")
    if PID_HTTP.exists():
        try:
            pid = int(PID_HTTP.read_text().strip())
            os.kill(pid, 0)
            print(f"http:   running (pid {pid})")
        except (ValueError, ProcessLookupError):
            print("http:   pid file stale")
    else:
        print("http:   stopped")
    for lang in LANGS[:1]:
        b = REPO / f"swad_{lang}"
        print(f"build:  {'LOCAL_DEV binaries present' if b.exists() else 'not built (run setup)'}")


def cmd_seed():
    seed()


def cmd_reset():
    cmd_stop()
    if DEPS.parent.exists():
        shutil.rmtree(DEPS.parent)
    if ROOT.exists():
        shutil.rmtree(ROOT)
    ROOT.mkdir(parents=True, exist_ok=True)
    log("All local dev data deleted (deps, db, binaries copy, state).")
    log("Re-run './swad_dev.py setup' to start over.")


def main():
    if len(sys.argv) < 2:
        print(__doc__)
        sys.exit(2)
    cmd = sys.argv[1]
    if cmd == "setup":
        cmd_setup()
    elif cmd == "build":
        build()
    elif cmd == "seed":
        cmd_seed()
    elif cmd == "start":
        cmd_start()
    elif cmd == "stop":
        cmd_stop()
    elif cmd == "status":
        cmd_status()
    elif cmd == "reset":
        cmd_reset()
    elif cmd == "serve":
        port = PORT
        args = sys.argv[2:]
        if "--port" in args:
            port = int(args[args.index("--port") + 1])
        serve(port)
    else:
        print(__doc__)
        sys.exit(2)


if __name__ == "__main__":
    main()
