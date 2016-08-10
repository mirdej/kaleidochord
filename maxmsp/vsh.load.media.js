var clip_path;
var clip_count;
var rec_path;
var rec_count;
var rec_slots;
var scale_path;
var movie_folder_path;
var current_scale = 0;
var files_not_found = 0;
var media = new Array();
var scales = new Array();
var idx = 0;

outlets = 2;

declareattribute("clip_path");
declareattribute("rec_path");
declareattribute("rec_slots");
declareattribute("scale_path");
declareattribute("movie_folder_path");


function conformpath(p) {
if (!p) return "unknown";
if (typeof p != "string") p = p.join(" ");

	 if (p.charAt(p.length-1) != "/") p += "/";
	return p;
}

function zeropad(i) {
	if (i < 10) return "00"+i;
	if (i < 100) return "0"+i;
	return i;
}

function process_scale(i) {
	var line;
	var pathitems = new Array;
	var i;
	var  temp;
	
	if (scales.length) {
	
		var local_drive_name = scale_path.split(":")[0];

		if (i < 0) i = (i + scales.length);	
		i %= scales.length;	
		current_scale = i;
		
		var f = new File (scales[i]);
		f.open();
		while (f.position != f.eof) {
			pathitems.length = 0;
			line = f.readline();
			// iview has old style semicolon paths. 
			// to accomodate for both types of paths, we'll explode by : and /
			line = line.split(":");
			for (i = 0; i < line.length; i++) {
				pathitems.push(line[i].split("/"));
			}
			
			// if this seems to reference a full path on the main hard-drive, keep the path, otherwise assume the file lies in the Movies folder
			if (pathitems[0] != local_drive_name) {
				line = movie_folder_path + pathitems[pathitems.length - 1];
			} else {
				pathitems[0] += ":";
				line = pathitems.join("/");
			}
			
			// see if file exists
			temp = new File(line);
				if (temp.eof == -1) {
					files_not_found++
				}
				temp.close();
			// push it on the string anyway
 			media.push(line);

		}
		f.close();
	}
}

function start(){
	outlet(1,"bang"); // clear filmstrip
	outlet(0,"menu","clear"); // clear scale menu
	var f;
	media.length = 0;
	scales.length = 0;
	files_not_found = 0;
	clip_path = conformpath(clip_path);	
	rec_path = conformpath(rec_path);	
	scale_path = conformpath(scale_path);
	movie_folder_path = conformpath(movie_folder_path);

	//-------------------- Look for scales in 
	f = new Folder (scale_path);
	while (!f.end) {
		 if (f.filename){			// js sees invisible files at beginning of folder???
 			scales.push(scale_path+f.filename)
 		//	post(scale_path+f.filename);post();
 			outlet(0,"menu","append",f.filename); // clear scale menu
  	 	}
  	 	f.next();
	}
	
	process_scale(current_scale);
	outlet(0,"menu","set",current_scale); // update scale menu

	
	//-------------------- Add clips in "Videobass Clips"
	f = new Folder (clip_path);
	clip_count = 0;
	while (!f.end) {
		 if (f.filename){			// js sees invisible files at beginning of folder???
 			media.push(clip_path+f.filename)
  	 	}
  	 	f.next();
  	}
  	clip_count = media.length;

  	
	//-------------------- Add recordings in "Videobass Recordings"
  	f = new Folder (rec_path);
	rec_count = 0;
	while (!f.end) {
		 if (f.filename){			// js sees invisible files at beginning of folder???
 			media.push(rec_path+f.filename);
 			rec_slots--;
 			rec_count++;
  	 	}
  	 	f.next();
  	}
	//-------------------- Add empty slots if applicable
	idx = rec_count;
	while (rec_slots > 0) {
	 	rec_slots--;
	 	media.push("Recording"+zeropad(idx))
	 	idx++;
	}
	
	
	idx = 0;
	outlet(0,"rec_count",rec_count);

	bang();
}

function set_scale(i) {
	current_scale = i;
	start();
}
function next_scale() {
	if (scales.length < 2) return;
	current_scale++;
	start();
}
function prev_scale() {
	if (scales.length < 2) return;
	current_scale--;
	start();
}
function bang()	{
	if (idx < media.length) {
		outlet(0,"add",media[idx]);
	} else {
		outlet(0,"clip_count",clip_count);
		outlet(0,"missing",files_not_found);
	}
	idx++;
}