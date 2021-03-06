#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){


	// macs by default run on non vertical sync, which can make animation very, very fast
	// this fixes that:

	ofSetVerticalSync( true );
	ofSetFrameRate( 60 );
	ofEnableSmoothing();
	// set background:

	ofBackground( 0,0,0 );
	bFullscreen	= false;
	useEnvelope = true;
	timeCounter			= 0;
	timeOfFrame			= ofGetSystemTime();
	timeOfLastFrame		= ofGetSystemTime(); 
	lastMousePressed = 0;
	// ------------------------------------ audio stuff:
	// 2 output channels,
	// 0 input channels
	// 44100 samples per second
	// 256 samples per buffer
	// 4 num buffers (latency)
	sampleRate 			= 44100;
	ofSoundStreamSetup(2,0,this, sampleRate,256, 4);

	showAudio = false;

	spawnFocusPoint = -1;
	spawnFocusRecorder = -1;

	hovering = NULL;
	recording = NULL;
	beatMod = 1;
	soundShape = 1;

	chromaticMode = false;
	selectionMode = false;
	holdSpawnMode = false;
	toConsole = false;
	holdSpawnMode = false;
	triggerAlwaysMode = false;
	helpMode = false; 


	// load images...
	triggerAlwaysImg.loadImage( "trigger_always.png" );
	triggerOnceImg.loadImage( "trigger_once.png" );
	helpTextImg.loadImage( "help_text.png" ); 

	// And buttons! 
	int X = 15; 
	int Y = 15; 
	int D1 = 10; 
	int D2 = 30; 
	
	beatBtns[0].init( X, Y, "beat_0.png" ); X += beatBtns[0].w + D1;
	beatBtns[1].init( X, Y, "beat_1.png" ); X += beatBtns[1].w + D1;
	beatBtns[2].init( X, Y, "beat_2.png" ); X += beatBtns[2].w + D1;
	beatBtns[3].init( X, Y, "beat_3.png" ); X += beatBtns[3].w + D1;
	beatBtns[4].init( X, Y, "beat_4.png" ); X += beatBtns[4].w + D1;
	beatBtns[5].init( X, Y, "beat_5.png" ); X += beatBtns[5].w + D1 + D2;
	
	shapeBtns[0].init( X, Y, "shape_flat.png" );      X += shapeBtns[0].w + D1;
	shapeBtns[1].init( X, Y, "shape_sinus.png" );     X += shapeBtns[1].w + D1;
	shapeBtns[2].init( X, Y, "shape_sawtooth.png" ); X += shapeBtns[2].w + D1;
	shapeBtns[3].init( X, Y, "shape_rectangle.png" );  X += shapeBtns[3].w + D1 + D2;
	
	newBtn.init ( X, Y, "new.png" );  X += newBtn.w + D1;
	loadBtn.init( X, Y, "load.png" ); X += loadBtn.w + D1;
	saveBtn.init( X, Y, "save.png" ); X += saveBtn.w + D1 + D2;
	
	selectBtn.init   ( X, Y, "select.png" );    X += selectBtn.w + D1;
	chromaticBtn.init( X, Y, "chromatic.png" ); X += chromaticBtn.w + D1;
	signalBtn.init   ( X, Y, "signal.png" );    X += signalBtn.w + D1;
	triggerBtn.init( X, Y, "trigger_once.png" );X += triggerBtn.w + D1;
	helpBtn.init     ( X, Y, "help.png" );      X += triggerBtn.w + D1 + D2;
	
	
	// now add all buttons to our buttons vector! 
	buttons.push_back( &beatBtns[0] ); 
	buttons.push_back( &beatBtns[1] ); 
	buttons.push_back( &beatBtns[2] ); 
	buttons.push_back( &beatBtns[3] ); 
	buttons.push_back( &beatBtns[4] ); 
	buttons.push_back( &beatBtns[5] ); 
	buttons.push_back( &shapeBtns[0] ); 
	buttons.push_back( &shapeBtns[1] ); 
	buttons.push_back( &shapeBtns[2] ); 
	buttons.push_back( &shapeBtns[3] ); 
	buttons.push_back( &newBtn ); 
	buttons.push_back( &loadBtn ); 
	buttons.push_back( &saveBtn ); 
	buttons.push_back( &selectBtn ); 
	buttons.push_back( &chromaticBtn ); 
	buttons.push_back( &signalBtn ); 
	buttons.push_back( &triggerBtn ); 
	buttons.push_back( &helpBtn ); 
	
	// and activate the ones that are usually activated! 
	beatBtns[1].selected = true; 
	shapeBtns[1].selected = true; 
	
	bpmRates[0] =   0; bpmLastTriggered[0] = 0; bpmTriggerNow[0] = false;
	bpmRates[5] =  20; bpmLastTriggered[1] = 0; bpmTriggerNow[1] = false;
	bpmRates[4] =  30; bpmLastTriggered[2] = 0; bpmTriggerNow[2] = false;
	bpmRates[3] =  40; bpmLastTriggered[3] = 0; bpmTriggerNow[3] = false;
	bpmRates[2] =  60; bpmLastTriggered[4] = 0; bpmTriggerNow[4] = false;
	bpmRates[1] = 120; bpmLastTriggered[5] = 0; bpmTriggerNow[5] = false;

	for( int i = 0; i < RECORDERS; i++ ){
		recorders[i].index = i;
	}

	
	showMenu = 18; 
	oldWidth = ofGetWidth(); 
	oldHeight = ofGetHeight();
}

//--------------------------------------------------------------
void testApp::update(){
	
	// First things first... scale everything... if needed! 
	if( ofGetWidth() != oldWidth || ofGetHeight() != oldHeight ){
		float w = ofGetWidth(); 
		float h = ofGetHeight(); 
		
		for( int i = 0; i < RECORDERS; i++ ){
			if( recorders[i].startTime != 0 ){
				for( int j = 0; j < recorders[i].pts.size(); j++ ){
					recorders[i].pts[j].pos.x *= w/oldWidth; 
					recorders[i].pts[j].pos.y *= h/oldHeight; 
					//recorders[i].volume *= (oldWidth+oldHeight)/(w+h);
				}
			}
		}
			
		oldWidth = w; 
		oldHeight = h; 
	}
	
	timeOfLastFrame = timeOfFrame; 
	timeOfFrame = ofGetSystemTime(); 
	
	// update all non-suicidal players. 
	// all the suicidal but not dead players 
	// need to be removed from the playersOfRecorders association vector
	// (it's late, that might not make much sense)
	for( int i = 0; i < PLAYERS; i++ ){
		if( !players[i].suicide ){
			players[i].update();
		}
		else if( players[i].suicide && !players[i].dead ){
			players[i].dead = true;
			vector<pointPlayer*> * them = &playersOfRecorders[ players[i].pr->index ];
			for( int j = 0; j < them->size(); j++ ){
				if( them->at(j) == &players[i] ){
					them->erase( them->begin()+j );
					break;
				}
			}
		}
	}


	// which bpm rates should be triggered?
	for( int i = 1; i < 6; i++ ){
		if( bpmLastTriggered[i] + 60.0/bpmRates[i] <= ofGetElapsedTimef() ){
			// This is not perfect, but should work for now...
			beatBtns[i].flash(); 
			bpmLastTriggered[i] += 60.0/bpmRates[i];
			bpmTriggerNow[i] = true;
		}
		else{
			bpmTriggerNow[i] = false;
		}
	}

	// trigger players!
	for( int i = 0; i < RECORDERS; i++ ){
		if( !recorders[i].bAmRecording && recorders[i].active() ){
			// Lines with beatMod 0 are only triggered if there is no
			// player already
			if( recorders[i].beatMod == 0 ){
				// what time should it trigger?
				// startTime + k*duration, 
				// where k is in Z
				// so basically we live in the group R/~duration
				int now = (timeOfFrame-recorders[i].startTime)% (int)(recorders[i].getDuration()*1000);
				int before = (timeOfLastFrame-recorders[i].startTime)%(int)(recorders[i].getDuration()*1000); 
				if( before > now ){
					// yep, we shall trigger! 
					pairUpWithAnyPlayer( &recorders[i] ); 
				}
				
			}
			// Lines with beatMod >0 are always triggered, except if they
			// are babysitters for other lines then they also are triggered
			// only if there is no other line.
			else if( recorders[i].beatMod > 0 ){
				// option 1: trigger always mode is on
				if( recorders[i].triggerAlways == true ){
					// right, so when should we trigger here? 
					// of course at startTime + k*60/bpm
					// same procedure as above: 
					int now = (timeOfFrame-recorders[i].startTime)%(int)(1000*60/bpmRates[recorders[i].beatMod]);
					int before = (timeOfLastFrame-recorders[i].startTime)%(int)(1000*60/bpmRates[recorders[i].beatMod]);
					if( before > now ){
						pairUpWithAnyPlayer( &recorders[i] ); 
					}
				}	
				// option 2: trigger always mode is off
				else{
					// when to trigger now? 
					// the only thing to figure out is the length of a loop. 
					// it is the duration + the waiting period when it syncs up with the bpm again! 
					// and that waiting period is obiously
					// bpmLoopLength - (duration%bpmLoopLength)
					int bpmLoopLength = 1000*60/bpmRates[ recorders[i].beatMod ]; 
					int wait = bpmLoopLength - ( (int)(1000*recorders[i].getDuration()) % bpmLoopLength ); 
					int total = (int)(1000*recorders[i].getDuration()) + wait; 
					
					int now = (timeOfFrame-recorders[i].startTime)%total;
					int before = (timeOfLastFrame-recorders[i].startTime)%total;
					if( before > now ){
						pairUpWithAnyPlayer( &recorders[i] ); 
					}
				}
			}
		}
	}

	// and then trigger kids!
	for( int i = 0; i < RECORDERS; i++ ){
		if( recorders[i].kids.size() > 0 ){

			pointRecorder * rec = &recorders[i];
			for( int j = 0; j < rec->kids.size(); j++ ){
				float when = rec->pts[rec->kidPointNr[j]].time;
				float duration = rec->pts[rec->pts.size()-1].time;
				double x1, x2;

				// is there a player that JUST played this?
				for( int k = 0; k < PLAYERS; k++ ){
					x2 = players[k].timeCounter;
					x1 = x2 - (double)players[k].diffTime;

					if( players[k].suicide == false && players[k].pr == rec && x1 <= when && when < x2 ){
						pairUpWithAnyPlayer( rec->kids[j] );
						k = PLAYERS; // "break" on the k-level
					}
				}
			}
		}
	}
	
	
	// Update buttons too! 
	for( int i = 0; i < buttons.size(); i++ ){
		buttons[i]->update( mouseX, mouseY ); 
	}
	
	// Now save if needed! 
	if( ( loadBtn.selected || saveBtn.selected ) && ( ofGetWidth() == ofGetScreenWidth() && ofGetHeight() == ofGetScreenHeight() ) ){
		bFullscreen = false; 
		ofSetFullscreen( false ); 
	}
	else if( loadBtn.selected ){
		load(); 
		loadBtn.selected = false; 
	}
	else if( saveBtn.selected ){
		save(); 
		saveBtn.selected = false; 
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	Tones::checkInit();
	if( chromaticMode ){
		Tones::draw();
	}

	pointRecorder *pr;
	ofPoint *pt;
	ofPoint *pt2;

	// draw relations between recorders
	glEnable(GL_LINE_STIPPLE);
	glLineStipple( 1, 0xF0F0 );
	ofSetColor( 80, 80, 80 );
	for( int i = 0; i < RECORDERS; i++ ){
		pr = &recorders[i];
		if( pr->startTime != 0 ){
			for( int j = 0; j < pr->kids.size(); j++ ){
				if( pr->kids[j]->pts.size() > 0 ){
					pt = &pr->pts[pr->kidPointNr[j]].pos;
					pt2 = &pr->kids[j]->pts[0].pos;
					ofNoFill();
					ofLine( pt->x, pt->y, pt2->x, pt2->y );
					ofFill();
					ofCircle( pt->x, pt->y, 2 );
				}
			}
		}
	}
	glDisable(GL_LINE_STIPPLE);

	// draw recorders themselves
	for( int i = 0; i < RECORDERS; i++ ){
		if( recorders[i].active() ){
			recorders[i].draw();
		}
	}

	ofSetRectMode(OF_RECTMODE_CORNER);
	ofSetColor( 255, 255, 255 );
	ofNoFill();
	ofSetLineWidth( 1 );

	for( int i = 0; i < buttons.size(); i++ ){
		buttons[i]->draw(); 
	}

	// Draw point players
	for( int i = 0; i < PLAYERS; i++ ){
		if( !players[i].suicide ){
			players[i].draw();
		}
	}

	if( selectionMode ){
		ofNoFill();
		glEnable(GL_LINE_STIPPLE);
		glLineStipple( 2, 0xF0F0 );
		ofSetColor( 255, 255, 255 );
		ofBeginShape();

		for( int i = 0; i < selectionPolyLength; i++ ){
			ofVertex( selectionPoly[i].x, selectionPoly[i].y );
		}

		ofEndShape( false );
		glDisable(GL_LINE_STIPPLE);
	}

	//string report = "nPts = " + ofToString(nPts) + "\ntotal time = " + ofToString(totalDuration, 3);
	//ofDrawBitmapString(report, 10, 10);
	for (vector<pointRecorder *>::iterator pr = selection.begin(); pr != selection.end(); ++pr ){
		ofFill();
		ofSetColor( 255, 0, 0 );
		float radius = 2+2*(*pr)->volume/0.1;
		ofCircle( (*pr)->pts[0].pos.x, (*pr)->pts[0].pos.y, radius );
	}

	if( hovering != NULL ){
		ofFill();
		ofSetColor( 255, 0, 0 );
		float radius = 2+2*hovering->volume/0.1;
		ofCircle( hovering->pts[0].pos.x, hovering->pts[0].pos.y, radius );
	}

	if( spawnFocusPoint >= 0 ){
		ofNoFill();
		ofSetColor( 255, 255, 0 );
		ofPoint *p = &recorders[spawnFocusRecorder].pts[spawnFocusPoint].pos;
		ofCircle( p->x, p->y, 5 );
	}

	if( showAudio ){
		ofPushMatrix(); 
		int graphW = 107;
		int graphH = 107;
		ofSetLineWidth( 1 );
		ofTranslate( 15, 50 );
		ofScale( graphW/256.0f, graphH/200.0f );
		ofSetRectMode( OF_RECTMODE_CORNER );
		ofSetColor( 0x333333 );
		ofNoFill();
		ofRect(   0, 0, 256, 200 );
		ofRect( 300, 0, 256, 200 );
		ofSetColor( 0xFFFFFF );
		for( int i = 0; i < 256; i++ ){
			ofLine(   0 + i, 100,   0 + i ,100 + lAudio[i] * 100.0f );
			ofLine( 300 + i, 100, 300 + i ,100 + rAudio[i] * 100.0f );
		}
		ofPopMatrix(); 
	}
	
	if( helpMode ){
		ofSetRectMode(OF_RECTMODE_CORNER);
		ofEnableAlphaBlending(); 
		ofSetColor( 255, 255, 255 ); 
		helpTextImg.draw( 0, 0 );
		ofDisableAlphaBlending(); 
	}
}

void testApp::pairUpWithAnyPlayer( pointRecorder * pr ){
	for( int i = 0; i < PLAYERS; i++ ){
		if( players[i].dead ){
			playersOfRecorders[pr->index].push_back( &players[i] );
			players[i].setup( pr );
			return;
		}
	}

	cout << "there's toooo much going on. i can't focus. aaaaah!" << endl;
}


//--------------------------------------------------------------
void testApp::keyPressed  (int key){
	glutModifiers = glutGetModifiers(); // can only be done in "core input callback"
	// set beat-mod using the keyboard
	if( key >= '0' && key <= '5' ){
		setBeatMod( key - '0' ); 
	}

	if( key == ' ' ){
		toConsole = !toConsole;
	}

	if( key == 10 /* enter */ ){
		for( int i = 1; i < 6; i++ ){
			bpmLastTriggered[i] = ofGetElapsedTimef();
		}
	}

	if( key == 'h' ){
		holdSpawnMode = true;
		return;
	}

	if( key == 'c' ){
		clear(); 
	}


	if( key == '-' ){
		if( hovering != NULL ){
			hovering->volume -= 0.01;
			if( hovering->volume < 0 ) hovering->volume = 0;
		}

		for (vector<pointRecorder *>::iterator pr = selection.begin(); pr != selection.end(); ++pr ){
			(*pr)->volume -= 0.01;
			if( (*pr)->volume < 0 ) (*pr)->volume = 0;
		}
	}

	if( key == '+' ){
		if( hovering != NULL ){
			hovering->volume += 0.01;
			if( hovering->volume > 1 ) hovering->volume = 1;
		}

		for (vector<pointRecorder *>::iterator pr = selection.begin(); pr != selection.end(); ++pr ){
			(*pr)->volume += 0.01;
			if( (*pr)->volume > 1 ) (*pr)->volume = 1;
		}
	}

	if( key == 'd' || key == 127 ){
		// delete the one recorder being hovered, eventually...
		if( hovering != NULL ){
			deleteRecorder( hovering );
		}

		// delete all recorders in the selection
		for( int i = 0; i < selection.size(); i++ ){
			deleteRecorder( selection[i] );
		}

		// empty selection!
		selection.clear();
		cleanup(); 
	}

	if( key == 'q' ) setSoundShape( 0 ); 
	if( key == 'w' ) setSoundShape( 1 ); 
	if( key == 'e' ) setSoundShape( 2 ); 
	if( key == 'r' ) setSoundShape( 3 ); 
	
	if( key == 'f' ){
		bFullscreen = !bFullscreen;
		ofSetFullscreen(bFullscreen);
	}


	if( key == 'a' ){
		setSignalVisualizer( !showAudio ); 
	}

	if( key == 's' ){ // fresh selection
		if( selectionMode )
			endSelection();
		else
			startSelection( false ); // start selection, don't append
	}

	if( key == 'S' ){ // add to selection
		if( selectionMode )
			endSelection();
		else
			startSelection( true ); // start selection, append! 
	}

	if( key == 'i' ){ // invert selection
		vector<pointRecorder * > temp;
		for( int i = 0; i < selection.size(); i++ ) temp.push_back( selection[i] );
		selection.clear();

		for( int i = 0; i < RECORDERS; i++ ){
			if( !binary_search( temp.begin(), temp.end(), &recorders[i] ) && recorders[i].active() ){
				selection.push_back( &recorders[i] );
			}
		}
	}

	if( key == 't' ){
		setChromaticMode( !chromaticMode ); 
	}


	if( key >= OF_KEY_F1 && key <= OF_KEY_F12 ){
		int nr = key - OF_KEY_F1;

		if( (glutModifiers & GLUT_ACTIVE_SHIFT ) == 0 ){
			// enable/disable
			setEnabled[nr] = !setEnabled[nr];
			for (vector<pointRecorder *>::iterator pr = sets[nr].begin(); pr != sets[nr].end(); ++pr ){
				(*pr)->enabled = setEnabled[nr];
			}

			selection.clear();
			if( setEnabled[nr] ){
				for (vector<pointRecorder *>::iterator pr = sets[nr].begin(); pr != sets[nr].end(); ++pr ){
					selection.push_back( (*pr) );
				}
				sort( selection.begin(), selection.end() );
				unique( selection.begin(), selection.end() );
			}
		}
		else{
			sets[nr].clear();
			for (vector<pointRecorder *>::iterator pr = selection.begin(); pr != selection.end(); ++pr ){
				sets[nr].push_back( (*pr) );
			}

			selection.clear();
			setEnabled[nr] = true;
		}
	}

	if( key == 'x' ){
		ofPoint * pt;
		for( int i = 0; i < RECORDERS; i++ ){
			if( recorders[i].active() ){
				pt = &recorders[i].pts[0].pos;
				if( pt->x < 0 || pt->y < 0 || pt->x > ofGetHeight() || pt->y > ofGetHeight() ){
					deleteRecorder( &recorders[i] );
				}
			}
		}
	}


	if( key == '.' ){
		// IO::save( this, "test.txt" );
		//save();
		saveBtn.selected = true; 
	}
	if( key == ',' ){
		// IO::save( this, "test.txt" );
		//load();
		loadBtn.selected = true; 
	}
	
	if( key == 9 /*tab*/ ){
		switch( showMenu ){
			case  0: showMenu = 10; break; 
			case 10: showMenu = 18; break; 
			case 18: showMenu = 0; break; 
			default: showMenu = 18; break; 
		}
		
		for( int i = 0; i < showMenu; i++ ){
			buttons[i]->visible = true; 
		}
		for( int i = showMenu; i < buttons.size(); i++ ){
			buttons[i]->visible = false; 
		}
	}
}

//--------------------------------------------------------------
void testApp::keyReleased  (int key){
	glutModifiers = glutGetModifiers(); // can only be done in "core input callback"


	if( key == 'h' ){
		holdSpawnMode = false;
	}
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	hovering = NULL;

	// are we really really close to a line?
	if( recording == NULL ){
		float dx, dy;
		for( int i = 0; i < RECORDERS; i++ ){
			if( recorders[i].active() && recorders[i].pts.size() > 0 ){
				// are we really close to the first point?
				float dx = mouseX - recorders[i].pts[0].pos.x;
				float dy = mouseY - recorders[i].pts[0].pos.y;

				if( sqrt( dx*dx + dy*dy ) < 5 ){
					hovering = &recorders[i];
					break;
				}
			}
		}
	}

	if( holdSpawnMode ){
		return;
	}

	spawnFocusPoint = -1;
	spawnFocusRecorder = -1;

	if( hovering == NULL && recording == NULL ){
		float minDistance = 100;
		float distance = 100;
		float dx, dy;


		for( int i = 0; i < RECORDERS; i++ ){
			if( recorders[i].active() && recorders[i].pts.size() > 0 ){
				for( int j = 0; j < recorders[i].pts.size(); j++ ){
					// are we really close to the first point?
					dx = mouseX - recorders[i].pts[j].pos.x;
					dy = mouseY - recorders[i].pts[j].pos.y;
					distance = sqrt( dx*dx + dy*dy );
					if( distance < 10 && distance < minDistance ){
						minDistance = distance;
						spawnFocusRecorder = i;
						spawnFocusPoint = j;
					}
				}
			}
		}
	}
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	if( selectionMode ){
		if( selectionPolyLength < 1000 ){
			selectionPoly[selectionPolyLength].x = x;
			selectionPoly[selectionPolyLength].y = y;
			selectionPolyLength++;
		}
		
		return;
	}
	
	
	if( chromaticMode ){
		y = Tones::snap( y );
	}

	// move, but only if shift is not pressed!
	if( hovering != NULL && ( glutModifiers & GLUT_ACTIVE_SHIFT ) == 0 ){
		bool moveKids = (glutModifiers & GLUT_ACTIVE_ALT) == 0;
		//cout << glutModifiers << endl;
		//cout << "move kids? " << moveKids << endl;
		float dx = -hovering->pts[0].pos.x + x;
		float dy = -hovering->pts[0].pos.y + y;
		moveRecorder( hovering, dx, dy, moveKids );

		for (vector<pointRecorder *>::iterator pr = selection.begin(); pr != selection.end(); ++pr ){
			if( *pr != hovering )
				moveRecorder( *pr, dx, dy, moveKids );
		}

		for( int i =0; i < RECORDERS; i++ ){
			if( recorders[i].active() )
				recorders[i].applyOffset();
		}
	}


	if( recording != NULL ){
		recording->addPoint( ofPoint(x,y,0) );
	}
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	glutModifiers = glutGetModifiers(); // can only be done in "core input callback"
	lastMousePressed = ofGetElapsedTimef();

	
	if( helpMode && y > 85 ){
		setHelpMode( false ); 
		return; 
	}
	
	if( chromaticMode ){
		y = Tones::snap( y );
	}


	// handle the buttons...
	if( mouseY <= 50 ){
		if( beatBtns[0].hover ){ setBeatMod( 0 ); return; }
		if( beatBtns[1].hover ){ setBeatMod( 1 ); return; } 
		if( beatBtns[2].hover ){ setBeatMod( 2 ); return; }
		if( beatBtns[3].hover ){ setBeatMod( 3 ); return; }
		if( beatBtns[4].hover ){ setBeatMod( 4 ); return; }
		if( beatBtns[5].hover ){ setBeatMod( 5 ); return; }
		
		if( shapeBtns[0].hover ){ setSoundShape( 0 ); return; }
		if( shapeBtns[1].hover ){ setSoundShape( 1 ); return; }
		if( shapeBtns[2].hover ){ setSoundShape( 2 ); return; }
		if( shapeBtns[3].hover ){ setSoundShape( 3 ); return; }
		
		if( newBtn.hover ){ clear(); return; }
		if( saveBtn.hover ){ saveBtn.selected = true; return; }
		if( loadBtn.hover ){ loadBtn.selected = true; return; }
		if( selectBtn.hover ){ 
			if( selectionMode )
				endSelection();
			else
				startSelection( false ); 
			
			return; 
		}
		if( chromaticBtn.hover ){ setChromaticMode( !chromaticMode ); return; }
		if( signalBtn.hover ){ setSignalVisualizer( !showAudio ); return; }
		if( triggerBtn.hover ){ setTriggerAlwaysMode( !triggerAlwaysMode ); return; }
		if( helpBtn.hover ){ setHelpMode( !helpMode ); return; }
	}
	
	
	
	if( selectionMode ){
		selectionPolyLength = 0;
		selectionPoly[selectionPolyLength].x = x;
		selectionPoly[selectionPolyLength].y = y;
		selectionPolyLength++;
		return;
	}

	if( !selectionMode && selection.size() > 0 ){
		// Only clear recorders if no line is hovered, otherwise it's
		// perfectly fine to delete!
		if( hovering == NULL ){
			selection.clear();
		}
	}


	if( hovering != NULL ){
		// usually we wanna abort here,
		// but if shift is pressed we generously
		// allow to continue for the parenting feature to work.
		if( ( glutModifiers & GLUT_ACTIVE_SHIFT ) == 0 ){
			return;
		}
	}

	// start recording!
	for( int i = 0; i < RECORDERS; i++ ){
		if( recorders[i].startTime == 0 && playersOfRecorders[i].size() == 0 && 
		    // and the crazy case: a babysitter still holds this thing! 
		   // this is not a perfect fix, but it helps! 
		    ( recorders[i].babysitter == NULL || playersOfRecorders[recorders[i].babysitter->index].size() == 0 )
		){
			recording = &recorders[i];
			recording->reset( this->beatMod );
			recording->startTime = ofGetSystemTime(); 
			recording->soundShape = soundShape;
			recording->triggerAlways = triggerAlwaysMode;

			// we're someone's spawn?
			if( spawnFocusRecorder >= 0 ){
				pointRecorder * rec = &recorders[spawnFocusRecorder];
				rec->kids.push_back( recording );
				rec->kidPointNr.push_back( spawnFocusPoint );
				ofPoint p = rec->pts[spawnFocusPoint].pos;
				recording->addPoint( p );
				recording->beatMod = -1; // this will never launch it's own players!
			}
			// parenting someone?
			else if( hovering ){
				recording->babysitting.push_back( hovering );
				recording->triggerAlways = false;
				hovering->babysitter = recording;
				hovering->babysitterX = 0; 
				hovering->babysitterY = 0; 
				// also add all other selected nodes. this should be fun! 
				for( vector<pointRecorder *>::iterator pr = selection.begin(); pr != selection.end(); ++pr ){
					if( (*pr) != hovering ){
						(*pr)->babysitter = recording; 
						(*pr)->babysitterX = (*pr)->pts[0].pos.x-mouseX; 
						(*pr)->babysitterY = (*pr)->pts[0].pos.y-mouseY; 
						recording->babysitting.push_back( (*pr) ); 
					}
				}
				
			}
			
			return;
		}
	}


	// NONE???
	recording = NULL;
}

//--------------------------------------------------------------
void testApp::mouseReleased(){
	glutModifiers = glutGetModifiers(); // can only be done in "core input callback"

	if( selectionMode && selectionPolyLength != 0 ){
		endSelection();
		return;
	}
	
	if( chromaticMode ){
		mouseY = Tones::snap( mouseY );
	}

	if( hovering != NULL && ofGetElapsedTimef() - lastMousePressed < 0.20 ){
		deleteRecorder( hovering );
		hovering = NULL;
		cleanup(); 
	}



	if( recording != NULL ){
		if( recording->pts.size() > 1 ){
			recording->addPoint( ofPoint(mouseX, mouseY,0) );
			recording->bAmRecording = false;
		}
		else{
			recording->reset( this->beatMod );

			// if this was anyone's babysitter then remove it!
			for( int i = 0; i < RECORDERS; i++ ){
				if( recorders[i].babysitter = recording ){
					recorders[i].babysitter = NULL;
				}
			}
		}

		timeCounter = 0;
		recording = NULL;
	}
}


//--------------------------------------------------------------
void testApp::audioRequested(float * output, int bufferSize, int nChannels){

	for (int i = 0; i < bufferSize; i++){
		output[i*nChannels    ] = 0;
		output[i*nChannels + 1] = 0;
	}

	for( int i = 0; i < PLAYERS; i++ ){
		if( !players[i].suicide ){
			if( players[i].pr->babysitting.size() > 0 ){
				vector<pointRecorder*> * vec = &players[i].pr->babysitting;
				ofPoint * targetPos = &players[i].currentPoint;
				for (vector<pointRecorder *>::iterator pr = vec->begin(); pr != vec->end(); ++pr ){
					this->moveRecorder( *pr, targetPos->x-(*pr)->pts[0].pos.x+(*pr)->babysitterX, targetPos->y-(*pr)->pts[0].pos.y+(*pr)->babysitterY, true );
				}
			}
			else{
				players[i].audioRequested( output, bufferSize, nChannels, useEnvelope );
			}
		}
	}

	for( int i =0; i < RECORDERS; i++ ){
		if( recorders[i].active() )
			recorders[i].applyOffset();
	}
	
	if( showAudio ){
		for( int i = 0; i < 256; i++ ){
			lAudio[i] = fmin( +2, fmax( -2, output[i*nChannels] ) );
			rAudio[i] = fmin( +2, fmax( -2, output[i*nChannels+1] ) );
		}
	}
	/*if( toConsole ){
		cout << "-----------------------" << endl;
		for( int i = 0; i < 256; i++ ){
			cout << output[i*nChannels] << ":" << output[i*nChannels+1] << ", ";
		}
		cout << endl;
	}*/

}


// ------------------------
void testApp::deleteRecorder( pointRecorder * rec ){
	rec->startTime = 0;
	pointRecorder * pr;

	for( int i = 0; i < RECORDERS; i++ ){
		pr = &recorders[i];
		if( pr == rec ){
			// delete all the recorders (if there are any)
			// that were triggered by this recorder
			deleteRecordersKids( pr );
		}
		else{
			// remove the recorder from the other recorder it was triggered by
			// this doesn't have to be the case, it might be...
			for( int j = 0; j < pr->kids.size(); j++ ){
				if( pr->kids[j] == rec ){
					pr->kids.erase( pr->kids.begin()+j );
					pr->kidPointNr.erase( pr->kidPointNr.begin()+j );
					j--;
				}
			}
			// also remove all references where this line was babysitted! 
			for( int j = 0; j < pr->babysitting.size(); j++ ){
				if( pr->babysitting[j] == rec ){
					pr->babysitting.erase( pr->babysitting.begin()+j ); 
					j--; 
				}
			}
		}
	}

	hovering = NULL;
	return;
}

// ------------------------
void testApp::deleteRecordersKids( pointRecorder * rec ){
	for( int j = 0; j < rec->kids.size(); j++ ){
		rec->kids[j]->startTime = 0;
		deleteRecordersKids( rec->kids[j] );
	}
}

// ------------------------
void testApp::moveRecorder( pointRecorder * rec, int dx, int dy, bool moveKids ){
	rec->offsetX = dx;
	rec->offsetY = dy;

	if( moveKids ){
		for( int i = 0; i < rec->kids.size(); i++ ){
			moveRecorder( rec->kids[i], dx, dy, true );
		}
	}
}

// ------------------------
void testApp::cleanup(){
	// remove all dead recorders from the selection
	vector<pointRecorder *>::iterator pr = selection.begin();
	while( pr != selection.end() ){
		if( (*pr)->startTime == 0 ){
			pr = selection.erase( pr ); 
		}
		else{
			pr++; 
		}
	}
	
	// remove all dead recorders from all patterns
	for( int i = 0; i < 12; i++ ){
		// remove all dead recorders from the selection
		vector<pointRecorder *>::iterator pr = sets[i].begin();
		while( pr != sets[i].end() ){
			if( (*pr)->startTime == 0 ){
				pr = sets[i].erase( pr ); 
			}
			else{
				pr++; 
			}
		}
	}
}

// ------------------------
void testApp::save(){
	#ifdef __APPLE__ // Apple code only!
	short fRefNumOut;
	FSRef output_file;
	OSStatus err;

	NavDialogCreationOptions options;
	NavGetDefaultDialogCreationOptions( &options );
	options.modality = kWindowModalityAppModal;
	//CFPropertyListRef propRef = CFPreferencesCopyAppValue( CFSTR("currentDirectory"), CFSTR("org.sd.soundythingie") );
	//options.saveFileName = (CFStringRef) propRef;
	//cout << "STR: " << CFStringGetCStringPtr(options.saveFileName, kCFStringEncodingUnicode) << endl;

	NavDialogRef dialog;
	err = NavCreatePutFileDialog(&options, '?', '?', NULL, NULL, &dialog);
	err = NavDialogRun(dialog);

	NavUserAction action;
	action = NavDialogGetUserAction( dialog );
	if (action == kNavUserActionNone || action == kNavUserActionCancel) {
		cout << "user canceled" << endl;
		return;
		// return 2;
	}

	// get dialog reply
	NavReplyRecord reply;
	err = NavDialogGetReply(dialog, &reply);
	if ( err != noErr )
		return; // return 2;

	if ( reply.replacing )
	{
		printf("need to replace\n");
	}

	AEKeyword keyword;
	DescType actual_type;
	Size actual_size;
	FSRef output_dir;
	err = AEGetNthPtr(&(reply.selection), 1, typeFSRef, &keyword, &actual_type,
					  &output_dir, sizeof(output_file), &actual_size);

	//printf("AEGetNthPtr returned %i\n", err );


	CFURLRef cfUrl = CFURLCreateFromFSRef( kCFAllocatorDefault, &output_dir );
	CFStringRef cfString = NULL;
	if ( cfUrl != NULL )
	{
		cfString = CFURLCopyFileSystemPath( cfUrl, kCFURLPOSIXPathStyle );
		CFRelease( cfUrl );
	}

	// copy from a CFString into a local c string (http://www.carbondev.com/site/?page=CStrings+)
	const int kBufferSize = 255;

	char folderURL[kBufferSize];
	Boolean bool1 = CFStringGetCString(cfString,folderURL,kBufferSize,kCFStringEncodingMacRoman);

	char fileName[kBufferSize];
	Boolean bool2 = CFStringGetCString(reply.saveFileName,fileName,kBufferSize,kCFStringEncodingMacRoman);

	// append strings together

	string url1 = folderURL;
	string url2 = fileName;
	string finalURL = url1 + "/" + url2;

	printf("url %s\n", finalURL.c_str());

	// cleanup dialog
	NavDialogDispose(dialog);
	//CFStringRef ref = CFStringCreateWithCString(NULL, finalURL.c_str(), kCFStringEncodingUnicode );
	//cout << ref << endl;
	//CFPreferencesSetAppValue(CFSTR("currentDirectory"), ref, CFSTR("org.sd.soundythingie") );
	//CFPreferencesAppSynchronize( CFSTR("org.sd.soundythingie") );

	save( finalURL ); 
	#endif

	#ifdef __WIN32__
	char szFileName[MAX_PATH] = "";

	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = "All Files (*.*)\0*.*\0";
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = 0;

	if(GetSaveFileName(&ofn)) {
	   save(string(szFileName));
	}
	#endif
}


// ------------------------
void testApp::load(){
	#ifdef __APPLE__ // Apple only!
	short fRefNumOut;
	FSRef output_file;
	OSStatus err;
	NavReplyRecord replyRecord;

	NavDialogCreationOptions options;
	NavGetDefaultDialogCreationOptions( &options );
	options.modality = kWindowModalityAppModal;
	//CFPropertyListRef propRef = CFPreferencesCopyAppValue( CFSTR("currentDirectory"), CFSTR("org.sd.soundythingie") );
	//options.saveFileName = (CFStringRef) propRef;
	//cout << "STR: " << CFStringGetCStringPtr(options.saveFileName, kCFStringEncodingUnicode) << endl;

	NavDialogRef dialog;

	err = NavCreateGetFileDialog(&options, NULL, NULL, NULL, NULL, NULL, &dialog);
	err = NavDialogRun(dialog);

	NavUserAction action;
	action = NavDialogGetUserAction( dialog );
	if (action == kNavUserActionNone || action == kNavUserActionCancel) {
		cout << "user canceled" << endl;
		return;
		// return 2;
	}



	// Get the reply
	err = NavDialogGetReply(dialog, &replyRecord);
	// If the user clicked "Cancel", just bail
	if ( err == userCanceledErr ){
		return;
	}

	// Get the file
	err = AEGetNthPtr(&(replyRecord.selection), 1, typeFSRef, NULL, NULL, &output_file, sizeof(FSRef), NULL);

	// Convert it to a CFURL
	CFURLRef cfUrl = CFURLCreateFromFSRef(NULL, &output_file);


	CFStringRef cfString = NULL;
	if ( cfUrl != NULL )
	{
		cfString = CFURLCopyFileSystemPath( cfUrl, kCFURLPOSIXPathStyle );
		CFRelease( cfUrl );
	}

	// copy from a CFString into a local c string (http://www.carbondev.com/site/?page=CStrings+)
	const int kBufferSize = 512;

	char folderURL[kBufferSize];
	Boolean bool1 = CFStringGetCString(cfString,folderURL,kBufferSize,kCFStringEncodingMacRoman);


	load( string( folderURL ) );

	#endif


	#ifdef __WIN32__
	char szFileName[MAX_PATH] = "";

	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = "All Files (*.*)\0*.*\0";
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = 0;

	if(GetOpenFileName(&ofn)) {
	   load(string(szFileName));
	}
	#endif
}


void testApp::clear( bool hard ){
	newBtn.activated(); 
	
	for( int i = 0; i < RECORDERS; i++ ){
		if( hard ) recorders[i].reset( 0 ); 
		recorders[i].startTime = 0;
	}
	
	if( hard ){
		for( int i = 0; i < PLAYERS; i++ ){
			players[i].suicide = true; 
			players[i].dead = true; 
		}
	}
	
	for( int i = 0; i < 12; i++ ){
		sets[i].clear();
	}
	
	
	hovering = NULL; 
	spawnFocusPoint = -1;
	spawnFocusRecorder = -1;	
	
	
	cleanup(); 
}


// ------------------------
bool testApp::save( string filename ){

	ofstream out( filename.c_str(), ios::out );
	for( int i = 0; i < RECORDERS; i++ ){
		out << "recorder:" << i << endl;
		recorders[i].save( out );
	}

	for( int i = 0; i < 12; i++ ){
		out << "set:";
		for( int j = 0; j < sets[i].size(); j++ ){
			out << sets[i][j]->index << " ";
		}
		out << "-1" << endl;
		out << "setEnabled:" << setEnabled[i] << endl;
	}

	out.close();

	return false;
}


// ------------------------
bool testApp::load( string filename ){
	ifstream in( filename.c_str(), ios::in );

	// hard reset! 
	clear( true ); 

	char cmd[64];

	int iRec = 0;
	int iSet = 0;
	int iSetEnabled = 0;
	while( !in.eof() ){
		Helpers::readCommand( cmd, in );
		if( 0 == strcmp( cmd, "recorder" ) ){
			recorders[iRec++].load( in, recorders, players );
		}
		if( 0 == strcmp( cmd, "set" ) ){
			int i = 0; in >> i;
			while( i != -1 && !in.eof() ){
				sets[iSet].push_back( &recorders[i] );
				in >> i;
			}
			iSet ++;
		}
		if( 0 == strcmp( cmd, "setEnabled" ) ){
			in >> setEnabled[iSetEnabled++];
		}
	}

	// great, now apply current set status!
	for( int i = 0; i < 12; i++ ){
		for (vector<pointRecorder *>::iterator pr = sets[i].begin(); pr != sets[i].end(); ++pr ){
			(*pr)->enabled = setEnabled[i];
		}
	}
	in.close();
}



// ------------------------
void testApp::setBeatMod( int mod ){
	beatBtns[beatMod].selected = false; 
	beatMod = mod; 
	beatBtns[beatMod].selected = true; 
	beatBtns[beatMod].activated(); 
}

// ------------------------
void testApp::setSoundShape( int newShape ){
	shapeBtns[soundShape].selected = false; 
	soundShape = newShape; 
	shapeBtns[soundShape].selected = true; 
	shapeBtns[soundShape].activated(); 
}

// ------------------------
void testApp::startSelection( bool append ){
	selectionMode = true; 
	selectionPolyLength = 0;
	
	selectBtn.selected = true; 
	selectBtn.activated();
	selectBtn.forceAlpha = 1; 

	
	if( !append ){
		selection.clear();
	}
}

// ------------------------
void testApp::endSelection(){
	selectionMode = false; 
	selectBtn.forceAlpha = -1; 
	selectBtn.selected = false; 
	
	for( int i = 0; i < RECORDERS; i++ ){
		pointRecorder *pr = &recorders[i];
		if( pr->active() && pr->pts.size() > 0 && Helpers::inPoly( selectionPoly, selectionPolyLength, pr->pts[0].pos ) ){
			selection.push_back( pr );
		}
	}
	
	// sort and shrink!
	sort( selection.begin(), selection.end() );
	unique( selection.begin(), selection.end() );
}

// ------------------------
void testApp::setChromaticMode( bool enabled ){
	chromaticMode = enabled; 
	
	chromaticBtn.selected = chromaticMode; 
	if( enabled ) chromaticBtn.activated(); 
}

// ------------------------
void testApp::setSignalVisualizer( bool enabled ){
	showAudio = enabled; 
	signalBtn.selected = enabled; 
	
	if( enabled ) signalBtn.activated(); 
}

// ------------------------
void testApp::setTriggerAlwaysMode( bool always ){
	triggerAlwaysMode = always; 
	triggerBtn.image = always? &triggerAlwaysImg:&triggerOnceImg; 
	triggerBtn.activated(); 
}

// ------------------------
void testApp::setHelpMode( bool enabled ){
	helpMode = enabled; 
	helpBtn.selected = enabled; 
	helpBtn.activated(); 
}