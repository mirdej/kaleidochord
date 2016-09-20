// inlets and outlets
inlets = 1;
outlets = 3;

o_preview 		= 0;
o_play		 	= 1;
o_messages 		= 2;

var paths = new Array()

var unfolded;
var current_clip = -1;


function set_clip (idx,path) {
	paths[idx] = path;
	current_clip = -1 // force reloading of clip
}

function msg_int (idx) {
	if (paths.length == 0) return;
	
	if (idx < 0) {error("clip out of range: ",idx); return;}
	if (idx >= paths.length) {error("clip out of range: ",idx); return;}
	if (idx == current_clip) return;
	
	current_clip = idx;
	outlet(o_messages,'playing_clip',idx);
	outlet(o_play,'read',paths[idx]);
}

function unfold(state) {
	if (state == unfolded) return;
	unfolded = state;
	
	if (unfolded == 1) {
		outlet(o_preview,'read',paths[current_clip]);
	} else {
	}
	
}