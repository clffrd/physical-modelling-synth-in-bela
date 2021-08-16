let buffer = [0,0,0,0,0];
let pickState = 0.9;
function setup() {

	//Create a canvas of dimensions given by current browser window
	createCanvas(windowWidth, windowHeight);
	pSlider = createSlider(0,1,0,0.01);
	dSlider = createSlider (0, 1, 0.5, 0.01);
	tSlider = createSlider (0.01, 10, 4, 0.01);
	sSlider = createSlider (0.900000, 0.999999, 0.950000, 0.000001);
	pickDriectionButton = createButton ('up/down');
	pickDriectionButton.mouseClicked(changePickStyle);
	changePickStyle();
	formatDOMElements();
}

function draw() 
{
	background(5,5,255,1);
	//Format and display a basic text in the browser's window.
	text('PickPosition', pSlider.x * 2 + pSlider.width, 35);
	text('Dampness', dSlider.x * 2 + dSlider.width, 35);
	text('DecayTime', tSlider.x * 2 + tSlider.width, 35);
	text('Stretch', sSlider.x * 2 + tSlider.width, 35);

	buffer[0] = pickState;
	buffer[1] = pSlider.value();
	buffer[2] = dSlider.value();
	buffer[3] = tSlider.value();
	buffer[4] = sSlider.value();

	Bela.data.sendBuffer(10,'float',buffer);
}

function formatDOMElements()
{
	pSlider.position((windowWidth-pSlider.width)/2,windowHeight/2+40);
	dSlider.position((windowWidth-dSlider.width)/2,windowHeight/2+60);
	tSlider.position((windowWidth-tSlider.width)/2,windowHeight/2+80);
	sSlider.position((windowWidth-sSlider.width)/2,windowHeight/2+120);
	button.position((windowWidth-button.width)/2,windowHeight/2-150);
	
	pickDriectionButton.position((windowWidth-button.width)/2,windowHeight/2-200);
	button.size(100,100);
	let col2 = color(0, 10, 10, 50);
	button.style('font-weight','bolder');
	button.style('border-radius', '50%');
	button.style('color', 'white');
}

function changePickStyle()
{
	pickState = 0.9 - pickState;
	return pickState;
}

