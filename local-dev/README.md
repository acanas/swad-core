# SWAD local development environment

Run the whole SWAD platform locally **without root, Docker, Apache or MySQL
server packages** — everything lives in `local-dev/root/` (git-ignored) and is
controlled by one script:

```bash
./local-dev/swad_dev.py setup     # one-time: build + DB + fictional seed data
./local-dev/swad_dev.py start     # start MariaDB + web server (background)
./local-dev/swad_dev.py stop      # stop everything
./local-dev/swad_dev.py status    # what is running
./local-dev/swad_dev.py seed      # re-create fictional data (idempotent)
./local-dev/swad_dev.py reset     # wipe ALL local dev data
```

Then open <http://localhost:8000/> (any language: `/en`, `/es`, ...).

## Fictional accounts (created by `seed`)

| Login     | Password        | Role                                   |
|-----------|-----------------|----------------------------------------|
| `prof1`   | `professor2026` | Professor + system superuser           |
| `student1`..`student5` | `student2026` | Students enrolled in the course    |

You can also log in with `@prof`, `@student1`, ... (nicknames) or the
`*@fictional.edu` email addresses. SWAD user IDs must contain at least one
digit, which is why the professor's ID is `prof1`, not `prof`.

Seeded hierarchy: country *Testland* → *Fictional University* →
*North Campus* → degree *Computer Science* → course
*"Introduction to the SWAD platform"* (`SWAD 101`, code 1).

## Architecture

- **Build**: `make LOCAL_DEV=1` compiles the CGI binaries against a new
  `LOCAL_DEV` block in `swad_config.h`. Paths/URLs are injected with
  `-DLOCAL_DEV_ROOT` / `-DLOCAL_DEV_PORT` (defaults: `local-dev/root`, port
  `8000`; override with `SWAD_DEV_ROOT` / `SWAD_DEV_PORT` env vars — but the
  port must match the build flag).
- **gSOAP**: the Arch `gsoap` package is extracted into
  `local-dev/root/deps/gsoap` (no sudo needed); `soapcpp2` generates
  `soap/soapC.c`, `soap/soapServer.c`, `soap/soapH.h`, `soap/swad.nsmap`.
- **Database**: `mariadbd` runs with datadir `local-dev/root/db`, socket
  `db/mysql.sock`, TCP `127.0.0.1:3306`. Schema from `sql/swad.sql`; user
  `swad`@`...` with a random password written to `cgi-bin/swad.cfg`.
- **Web**: `swad_dev.py serve` is a small threaded CGI server. `/en`, `/es`,
  ... map to `cgi-bin/swad_<lang>` (cwd = cgi-bin so `swad.cfg`, `foot.html`,
  `chat.html`, helper commands resolve). Everything else is served statically
  from `local-dev/root/www` (`/swad/*` → symlinks into the repo assets).
- **Sessions**: SWAD uses `?ses=` URL parameters — no cookies needed.

## Source patches applied for this environment

- `swad_config.h`: new `LOCAL_DEV` install block (plain HTTP, user-space paths).
- `Makefile`: `LOCAL_DEV=1` adds `-DLOCAL_DEV*`, gSOAP include/lib paths.
- `swad_follow.c`, `swad_API.c`, `swad_building.c`, `swad_forum.c`,
  `swad_mark.c`, `swad_question_database.c`, `swad_room.c`,
  `swad_timeline_notification.c`: added missing `#include <stdlib.h>`
  (GCC ≥14 treats implicit `malloc` declarations as errors).
- `swad_user_database.c`: `FROM_UNIXTIME(0)` returns NULL in MariaDB ≥11,
  so the "last notification access" epoch is written as `1` instead of `0`.

## Known limitations

- `fotomaton` (face detection) is not built — it needs OpenCV. Photo
  averaging (`foto_promedio`/`foto_mediana`) binaries are included.
- MathJax is not installed (`/swad/mathjax/*` will 404); formulas in course
  content won't render. Download `mathjax-4.1.0.tgz` from the install docs
  into `local-dev/root/www/swad/mathjax/` if needed.
- Email/SMTP is a stub (`swad_smtp.py` will fail to deliver; fine for dev).
- `make LOCAL_DEV=1` must not be run with `-j` across all languages — the
  per-language objects share filenames; `swad_dev.py build` handles it.

## Logs

`local-dev/root/logs/http.log` (web server + CGI stderr),
`local-dev/root/logs/mysqld.log`, `local-dev/root/logs/mariadb.err.log`.
