function paint()
{
	var val = box.getvalueof()[0]; // this is an array of size 1
	var viewsize = mgraphics.size;
	var valrange = box.getattr("size");
	var mode = box.getattr("mode");
	var degrees = box.getattr("degrees");
	var thickness = box.getattr("thickness");
	var width = viewsize[0];
	var height = viewsize[1];
	var start,end;

    start = -1.57 - (degrees / 360 * 3.14);
    end = -1.57 + (degrees / 360 * 3.14);

	
	mgraphics.relative_coords = 1;
	mgraphics.set_source_rgba(box.getattr("bgcolor"));
	mgraphics.rectangle(0, 0, width, height);
	mgraphics.fill();
	
	mgraphics.set_line_width(.15 * thickness/100.)
	mgraphics.set_source_rgba(box.getattr("outlinecolor"));
	mgraphics.arc(0,0,.9,start,end)
	mgraphics.stroke();
	
	mgraphics.set_source_rgba(box.getattr("needlecolor"));

	if (mode) {
		val = val * 2. - 1.
		if (val < 0) {
			end = -1.57 + (270. / 360 * 3.14 * val);
			mgraphics.arc_negative(0,0,.9,-1.57,end)
		} else {
			end = -1.57 + (270. / 360 * 3.14 * val);
			mgraphics.arc(0,0,.9,-1.57,end)
		}
	} else {
			
			mgraphics.arc(0,0,.9,2.355,(270. / 360 * 6.28 * val)+2.355);
	}
	mgraphics.stroke();

	
}