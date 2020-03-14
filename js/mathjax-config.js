MathJax = {
		/* Bug fix for 3.0.1 version.
		   Test and delete if a new version works.
		   https://www.bountysource.com/issues/88419745-processescapes-set-to-true-doesn-t-work */
	    startup: {
	        pageReady() {
	          const options = MathJax.startup.document.options;
	          const BaseMathItem = options.MathItem;
	          options.MathItem = class FixedMathItem extends BaseMathItem {
	            assistiveMml(document) {
	              if (this.display !== null) super.assistiveMml(document);
	            }
	          };
	          return MathJax.startup.defaultPageReady();
	        }
	      },
		/* end of bug fix */
  tex: {
    /* inlineMath: [['$', '$'], ['\\(', '\\)']] // Uncomment to use $...$ for inline math */
    inlineMath: [['\\(', '\\)']]
  },
  svg: {
    fontCache: 'global'
  }
};
