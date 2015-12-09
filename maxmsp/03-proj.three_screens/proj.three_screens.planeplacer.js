var MAX_PLANES = 12;
var MAX_DELAY = 75;
var planes = new Array();
var delay_fact = 1;

function master_delay(f) {
	delay_fact = f;
	for (var i = 1; i < MAX_PLANES; i++) {
		planes[i].postDelay();
	}
}

function set_delay(i,d) {
	i--;
	planes[i].delay = d;
	planes[i].postDelay();
}

function filmstrip(spread,size,delay) {
	for (var i = 0; i < MAX_PLANES; i++) {
		planes[i].position = [(i / MAX_PLANES)*spread - spread/2. ,	0.];
		planes[i].scale = size;		
		planes[i].delay = i*delay;
		planes[i].blend = 1.;
		planes[i].enable = 1;
	}
	bang();
}


function moveall(x,y) {
		for (var i = 0; i < MAX_PLANES; i++) {
			planes[i].position[0] += x;
			planes[i].position[1] += y;
			planes[i].postPos();
		}
}



function bang() { 
	for (var i = 0; i < MAX_PLANES; i++){
		planes[i].postAll();
	}
}

function randomDelays () {	
	for (var i = 1; i < MAX_PLANES; i++) {
		planes[i].delay = Math.random()*MAX_DELAY;
		planes[i].postDelay();
	}
}



function vPlane(n) {
	this.index = n+1;
	this.position = [0. , 0.];
	this.scale =  .5;
	this.delay =  2;
	this.blend = 1;
	this.source = 1;
	this.flip = [0 , 0]
	
	this.postPos = function() {
		outlet(0,'poly~.'+this.index+'::position',this.position);
	}
	this.postScale = function() {
		outlet(0,'poly~.'+this.index+'::scale',this.scale);
	}
	this.postDelay = function() {
		outlet(0,'poly~.'+this.index+'::delay',parseInt(this.delay * delay_fact));
	}
	this.postBlend = function() {
		outlet(0,'poly~.'+this.index+'::blend',this.blend);
	}
	this.postSource = function() {
		outlet(0,'poly~.'+this.index+'::source',this.source);
	}
	this.postFlip = function() {
		outlet(0,'poly~.'+this.index+'::flip',this.flip);
	}
	
	this.postAll = function() {
		this.postPos();
		this.postScale();
		this.postDelay();
		this.postBlend();
		this.postSource();
		this.postFlip();
	}
}


function init() {
	for (var i = 0; i < MAX_PLANES; i++) {
		planes[i] = new vPlane(i);
	}
}


init();