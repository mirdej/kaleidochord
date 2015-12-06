var pos = [-5.6, -.75];
var size = 3;
var c1 = 0.2
var c2 = 0.4
var l1 = .05

function bang() {
	outlet(0,'gllinewidth',1);
	outlet(0,'glcolor',c1,c1,c1,1);

	outlet(0,'moveto',pos);
	outlet(0,'line',size,0);
	var si = -.1;
	for (var i = 0; i < 8; i++) {
		outlet(0,'line',0,-l1);
		outlet(0,'move',-(size/8),l1);
	}
	
	outlet(0,'move',0,-l1);
	outlet(0,'line',0,2*l1);
	outlet(0,'move',size/2,0);
	outlet(0,'line',0,-l1);
	outlet(0,'move',size/2,0);
	outlet(0,'line',0,l1);
}

function msg_float(f) {
	outlet(0,'reset');
	outlet(0,'gllinewidth',4);
	outlet(0,'glcolor',c2,c2,c2,1);
	outlet(0,'moveto',pos[0]+(size/2),pos[1]);
	outlet(0,'line',f*size/2,0);
	bang();
}