/*

vmix 2d dial

arguments: name value style

*/

sketch.default2d();
var val = 0.5;
var style = 1;
var name = "";
var vbrgb = [0.,0.,0.,1.];
var vfrgb = [0.1,0.5,1.,1.];
var vrgb2 = [0.7,0.7,0.7,1.];
var last_x = 0;
var last_y = 0;

// process arguments
if (jsarguments.length>1)
	name = jsarguments[1];
if (jsarguments.length>2)
	val = jsarguments[2];
if (jsarguments.length>3)
	style = jsarguments[3];

draw();

function draw()
{

if (style==2)  vfrgb = [0.1,1.,1.,1.];

	var theta;
	var width = box.rect[2] - box.rect[0];
	
	with (sketch) {
		shapeslice(180,1);
		// erase background
		glclearcolor(vbrgb[0],vbrgb[1],vbrgb[2],vbrgb[3]);
		glclear();			
		moveto(0,.25);
		// fill bgcircle
		glcolor(vrgb2);
		//circle(0.8);
		// draw arc outline
		glcolor(vfrgb[0],vfrgb[1],vfrgb[2],.25);
		circle(0.9,225,225-270);
	
		glcolor(vfrgb);
		if (style==2) {circle(0.9,225,225-val*270);}
		else {
		  circle(0.9,90,90-(val-0.5)*270);
		}
		// fill arc			
		glcolor(vbrgb);
		circle(0.7);
		moveto (0.,-.95);
						glcolor(1.,1.,1.,1.);
	 textalign ("center", "bottom");
	 text(name);

	//	circle(0.7,-90-val*360,-90);						
		// draw rest of outline
	/*	if (width<=32)
			gllinewidth(1);
		else
			gllinewidth(2);
		glcolor(0,0,0,1);
		moveto(0,0);
		lineto(0,-0.8);
		moveto(0,0);
		theta = (0.75-val)*2*Math.PI;
		lineto(0.8*Math.cos(theta),0.8*Math.sin(theta)); */
	}
}

function bang()
{
	draw();
	refresh();
	outlet(0,val);
}

function msg_float(v)
{
	val = Math.min(Math.max(0,v),1);
	notifyclients();
	bang();
}

function set(v)
{
	val = Math.min(Math.max(0,v),1);
	notifyclients();
	draw();
	refresh();
}

function fsaa(v)
{
	sketch.fsaa = v;
	bang();
}

function frgb(r,g,b)
{
	vfrgb[0] = r/255.;
	vfrgb[1] = g/255.;
	vfrgb[2] = b/255.;
	draw();
	refresh();
}

function rgb2(r,g,b)
{
	vrgb2[0] = r/255.;
	vrgb2[1] = g/255.;
	vrgb2[2] = b/255.;
	draw();
	refresh();
}

function brgb(r,g,b)
{
	vbrgb[0] = r/255.;
	vbrgb[1] = g/255.;
	vbrgb[2] = b/255.;
	draw();
	refresh();
}

function setvalueof(v)
{
	msg_float(v);
}

function getvalueof()
{
	return val;
}

// all mouse events are of the form: 
// onevent <x>, <y>, <button down>, <cmd(PC ctrl)>, <shift>, <capslock>, <option>, <ctrl(PC rbutton)>
// if you don't care about the additonal modifiers args, you can simply leave them out.
// one potentially confusing thing is that mouse events are in absolute screen coordinates, 
// with (0,0) as left top, and (width,height) as right, bottom, while drawing 
// coordinates are in relative world coordinates, with (0,0) as the center, +1 top, -1 bottom,
// and x coordinates using a uniform scale based on the y coordinates. to convert between screen 
// and world coordinates, use sketch.screentoworld(x,y) and sketch.worldtoscreen(x,y,z).

function onclick(x,y,but,cmd,shift,capslock,option,ctrl)
{
	// cache mouse position for tracking delta movements
	last_x = x;
	last_y = y;
}
onclick.local = 1; //private. could be left public to permit "synthetic" events

function ondrag(x,y,but,cmd,shift,capslock,option,ctrl)
{
	var f,dy;
	
	// calculate delta movements
	dy = y - last_y;
	if (shift) { 
		// fine tune if shift key is down
		f = val - dy*0.001; 
	} else {
		f = val - dy*0.01;
	}
	msg_float(f); //set new value with clipping + refresh
	// cache mouse position for tracking delta movements
	last_x = x;
	last_y = y;
}
ondrag.local = 1; //private. could be left public to permit "synthetic" events

function ondblclick(x,y,but,cmd,shift,capslock,option,ctrl)
{
	last_x = x;
	last_y = y;
	msg_float(0); // reset dial?
}
ondblclick.local = 1; //private. could be left public to permit "synthetic" events

function forcesize(w,h)
{
	if (w!=h) {
		h = w;
		box.size(w,h);
	}
}
forcesize.local = 1; //private

function onresize(w,h)
{
	forcesize(w,h);
	draw();
	refresh();
}
onresize.local = 1; //private
