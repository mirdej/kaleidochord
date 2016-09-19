// inlets and outlets
inlets = 1;
outlets = 4;

o_matrix 		= 0;
o_matrixset 	= 1;
o_messages 		= 2;
o_bang	 		= 3;

var main_height = 210;
var main_width;

var aspect 	= 4/3;
var IMAGE_SPACING = 5;

var main_height = 210;
var main_width;

var image_width;
var image_height;

var clip_count = 0;

var filmstrip_width = 0;
var x_off;
var home ="Videobass:/Users/videobass/"


declareattribute("home")
declareattribute("aspect")

// Jitter matrices to work with 
var loader_matrix = new JitterMatrix(4, "char", 1,1); // we'll change size later anyway
var filmstrip_matrix = new JitterMatrix(4, "char", 1, 1); // we'll change size later anyway
var filmstrip_backup = new JitterMatrix(4, "char", 1, 1); // we'll change size later anyway

//Render objects to draw empty frames
var placeholder_render = new JitterObject ("jit.gl.render", "blank") ;
    placeholder_render.erase_color = [0., 0., 0.5, 1.] ;
    placeholder_render.blend_enable = 1 ;
	
var placeholder_text = new JitterObject ("jit.gl.text", "blank") ;
    placeholder_text.blend_enable = 1 ;
	placeholder_text.depth_enable = 0 ;
    placeholder_text.color = [1., 1., 1., 1.] ; 
    placeholder_text.position = [0., 0., 0.] ;
    placeholder_text.align = 1 ;
    placeholder_text.font ("Lucida Grande") ;
	placeholder_text.face ("bold") ;
    placeholder_text.size (24) ;
	
var placeholder_matrix = new JitterObject ("jit.matrix", "blank") ;         // rendering across a jit.matrix.
	
function strippath(p) {
	var items = p.split('/');
	var n = items.length - 1;
	return items[n];
}

function calc_aspect() {
	main_width = parseInt(main_height * aspect);
	image_width 	= main_width / 2;
	image_height	= main_height / 2;
	loader_matrix.dim = [main_width,main_height];
}

function render_placeholder(name) {
	placeholder_matrix.dim = [main_width,main_height];
	placeholder_render.erase () ;
	placeholder_text.text(name);
	placeholder_render.drawswap () ;
}


function add_clip(path) {
		calc_aspect();

		clip_count++;
		outlet(o_messages,'count',clip_count);
		outlet(o_messages,'message','Adding Clip:',strippath(path));
		
// backup old filmstrip
		filmstrip_backup.dim = filmstrip_matrix.dim;
		filmstrip_backup.frommatrix(filmstrip_matrix.name);

// make room for new picture
		filmstrip_width = (clip_count+8) * (image_width + IMAGE_SPACING);
		filmstrip_width -= IMAGE_SPACING;
		filmstrip_matrix.dim = [filmstrip_width,image_height];
		filmstrip_matrix.usedstdim = 1;
	//	filmstrip_matrix.setall(255,60,60,60);
		outlet(o_matrix,'dim',filmstrip_matrix.dim);

// copy backup matrix back in
		filmstrip_matrix.dstdimstart = [0,0];
		filmstrip_matrix.dstdimend = [filmstrip_backup.dim[0]-1,filmstrip_backup.dim[1]-1];
		filmstrip_matrix.frommatrix(filmstrip_backup.name);

// add new picture
		set_clip(clip_count - 1, path);
		outlet(o_bang,'bang');
}


function set_clip(idx,path) {
		outlet(o_messages,'set_clip',idx,path);

		if (idx < 0 || idx > clip_count - 1) {
			error("Filmstrip: Clip out of range: ",idx);
			return;
		}
		
		clip_name = path.split("/");
		clip_name = clip_name[clip_name.length -1];

// see if file exists
		f = new File(path);
		if (f.isopen) {
			loader_matrix.importmovie(path); //load movie thumbnail
		} else {
			render_placeholder(clip_name);	//create placeholder thumbnail
			loader_matrix.frommatrix(placeholder_matrix.name);
		}
		outlet(o_messages,'names',clip_name,"cr");

// add thumnail to matrixset (middle view)
		outlet(o_matrixset,'index',idx);
		outlet(o_matrixset, "jit_matrix", loader_matrix.name);

// add new picture
		x_off = (4 + idx) * (image_width + IMAGE_SPACING);
		filmstrip_matrix.dstdimstart = [x_off,0];
		filmstrip_matrix.dstdimend = [x_off + image_width,filmstrip_matrix.dim[1]];
		filmstrip_matrix.frommatrix(loader_matrix.name);
		
		outlet(o_matrix, "jit_matrix", filmstrip_matrix.name);
		outlet(o_matrix,'bang');
}

function clear() {
	clip_count = 0;
	calc_aspect();
//	outlet(o_matrixset,'dim',main_width,main_height);
//	outlet(o_matrixset,'matrixcount',600);
	filmstrip_backup.dim = [0,0];	
	filmstrip_matrix.dim = [0,0];
	
	outlet(o_messages,'names','clear');
	outlet(o_messages,'count',0);
	outlet(o_messages,'active',1); /// ??????? K
	
	//render_placeholder("empty");
	//	outlet(o_matrixset,'index',0);
	//	outlet(o_matrixset, "jit_matrix", placeholder_matrix.name);
}

function loadbang() {
	clear();
}
