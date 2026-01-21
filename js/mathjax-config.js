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
    inlineMath: [                    // start/end delimiter pairs for in-line math
      // ['$', '$'],                 // uncomment to use $...$ for inline math, problem: users must write \$ for dollar symbol
      ['\\(', '\\)']
    ],
    displayMath: [                   // start/end delimiter pairs for display math
      ['$$', '$$'],
      ['\\[', '\\]']
    ],
    processEscapes: true,            // use \$ to produce a literal dollar sign
  },
  svg: {
    fontCache: 'global'
  }
};
