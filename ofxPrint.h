#pragma once

#include "ofMain.h"

namespace ofx
{
	namespace mtl
	{
		//--------------------------------------------------------------
		class PrintEventArgs 
			: public ofEventArgs
		{
		public:
			PrintEventArgs(const string& printerName = "", float duration = -1);

			string printerName;
			PrintMode mode;
			string path;
			ofImage image; 
			float duration;
			bool bSuccess;
		};

		//--------------------------------------------------------------
		enum PrintMode
		{
			OFX_PRINT_ERROR,
			OFX_PRINT_IMAGE_PATH,
			OFX_PRINT_IMAGE_OF,

			OFX_PRINT_NUM_MODES
		};

		//--------------------------------------------------------------
		class Print
			: public ofThread
		{
		public:
			Print();
			~Print();

			void setup(const string& name);

			void setPrinterName(const string& name);
			const string& getPrinterName();

			void printImage(const string& path, bool bThreaded = true);
			void printImage(const ofImage& image, bool bThreaded = true);

			void threadedFunction();

			void update(ofEventArgs& args);

			ofEvent<PrintEventArgs> printCompleted;

		private:
			void printImageImpl(PrintEventArgs *args);

			string _name;

			std::deque<PrintEventArgs *> printQueue;
			std::deque<PrintEventArgs *> resultQueue;
		};
	}
}

typedef ofx::mtl::PrintEventArgs ofxPrintEventArgs;
typedef ofx::mtl::PrintMode ofxPrintMode;
typedef ofx::mtl::Print ofxPrint;
