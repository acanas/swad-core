// "myAwesomeDropzone" is the camelized version of the HTML element's ID
Dropzone.options.myAwesomeDropzone =
	{
	maxFiles: 10,
	parallelUploads: 10,
	//forceFallback: true,
	init: function()
	{
		var filesAdded = 0;
	
		this.on("addedfile", function(file)
		{
			filesAdded++;
			//alert("Added file.");
		});

		// If you don't want to automatically close if there was an error, you can handle
		// the success and error events separately
		this.on("complete", function(file)
		{
			filesAdded--;
			if (filesAdded == 0)
			{
				// Do what you have to do.
				//alert ("Hecho");
				//setTimeout(function(){document.getElementById("form_upload_done").submit();}, 2000);
			}
		});
	},
	fallback: function()
	{
		document.getElementById('dropzone-upload').style.display='none';
		document.getElementById('classic-upload').style.display='block';
	}
};
