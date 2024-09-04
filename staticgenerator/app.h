#pragma once

typedef struct IUnknown IUnknown;

#include <wx/wx.h>
#include "main.h"


/*********************************
             Classes
*********************************/

class App : public wxApp
{
	private:
		Main* m_Frame = nullptr;
        
	protected:
    
	public:
		App();
		~App();
		virtual bool OnInit();
        virtual bool OnExceptionInMainLoop() override
        {
            try { throw; }
            catch(std::exception &e)
            {
                wxMessageBox(e.what(), "C++ Exception Caught");
            }
            return true;   // continue on. Return false to abort program
        }
};