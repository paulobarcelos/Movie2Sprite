#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofDirectory dir;
	ofVideoPlayer player;
	
	int edge = 2;
	
	ofxXmlSettings settings;	
	settings.loadFile("settings.xml");
	

	ofVec2f movieSize(settings.getValue("movieSize:width", 128) + edge,
					  settings.getValue("movieSize:height", 128) + edge);
	ofVec2f texSize(settings.getValue("texSize", 1024),
					settings.getValue("texSize", 1024));
	float duration = settings.getValue("duration", 1.0);
	float fps = settings.getValue("fps", 1.0);
	
	int numFrames = duration * fps;
	int numMovies = dir.listDir("movies");
	int totalFrames = numFrames * numMovies;
		
	float movieRatio = movieSize.x / movieSize.y;
	
	float texArea = texSize.x * texSize.y;	
	float fitArea = texArea / (float)totalFrames;
	
	float fitHeight = sqrt(fitArea/movieRatio);
	float fitWidth = fitArea/fitHeight;
	
	ofVec2f fitSize(fitWidth,
					fitHeight);
	
	ofVec2f numUnits(ceil(texSize.x / fitSize.x),
					ceil(texSize.y / fitSize.y));	
	
	ofVec2f size = texSize / numUnits;
	ofVec2f scale = movieSize / size;
	
	ofFile::removeFile("sprite.js");
	ofstream js;
	js.open (ofToDataPath("sprite.js").c_str(), std::ios::out | std::ios::app);
	js << "var sprite={coordinates:[";
	
	ofFbo fbo;
	fbo.allocate(texSize.x, texSize.y);
	fbo.begin();
	ofClear(0, 0, 0, 0);
	fbo.end();
	
	float column = -1;
	float row = -1;
	int frameIndex = 0;
	
	for (float i = 0; i < numMovies; i++) {
		player.loadMovie(dir.getPath(i));
		cout << dir.getPath(i) << endl;
		int playerNumFrames = player.getTotalNumFrames();
		float offset = playerNumFrames / numFrames;
		
		js << "[";
		for (float j = 0; j < numFrames; j++) {
			column = frameIndex % (int)numUnits.x;
			if(column == 0) row++;
			
			js << "\"-" << (column * movieSize.x) << "px -" << (row * movieSize.y) << "px\"";
			if(j < numFrames-1) js << ",";
			
			player.setFrame((int)((float)j * offset)+1);
			player.update();
			fbo.begin();
			player.draw(floor(column * size.x), floor(row * size.y), ceil(size.x), ceil(size.y));
			fbo.end();
			
			frameIndex++;
		}
		js << "]";
		if(i < numMovies-1) js << ",";

	}
	
	js << "]";
	js << ",duration:" << duration;
	js << ",image:\"sprite.png\"";
	js << ",numFrames:" << numFrames;
	js << ",numMovies:" << numMovies;
	js << ",width:" << movieSize.x - edge;
	js << ",height:" << movieSize.y - edge;
	js << ",backgroundSize:\"" << (texSize.x * scale.x) << "px " << (texSize.y * scale.y)<< "px\"";
	js << "};";
	js.close();
	
	ofPixels pixels;
	pixels.allocate(texSize.x, texSize.y, 4);
	fbo.readToPixels(pixels);
	ofSaveImage(pixels, "sprite.png");
	
	ofExit(0);
}