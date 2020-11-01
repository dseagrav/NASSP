/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2003-2006 

  Project Apollo Configurator

  Project Apollo is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  Project Apollo is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Project Apollo; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  See http://nassp.sourceforge.net/license/ for more details.

  **************************************************************************/

#pragma warning(disable : 4996 ) 

#define STRICT 1
#define ORBITER_MODULE
// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "orbitersdk.h"
#include <commctrl.h>
#include "resource.h"
#include <stdio.h>

// ==============================================================
// Some global parameters

#define MAX_TABNUM 3

// file name for storing custom parameters
static const char *cfgfile = "ProjectApollo/Saturn5.launchpad.cfg";

class ProjectApolloConfigurator;

///
/// \ingroup DLLsupport
///
static struct {
	HINSTANCE hInst;	
	HWND hDlgTabs[MAX_TABNUM];
	ProjectApolloConfigurator *item;

	int Saturn_MainPanelSplit;
	int Saturn_GNSplit;
	int Saturn_LowRes;
	int Saturn_FDAIDisabled;
	int Saturn_FDAISmooth;
	int Saturn_RHC;
	int Saturn_THC;
	int Saturn_RSL;
	int Saturn_TSL;
	int Saturn_RHCTHCToggle;
	int Saturn_RHCTHCToggleId;
	int Saturn_MaxTimeAcceleration;
	int Saturn_MultiThread;
	int Saturn_VAGCChecklistAutoSlow;
	int Saturn_VAGCChecklistAutoEnabled;
	int Saturn_VcInfoEnabled;
} gParams;


// ==============================================================
// A class defining the new launchpad parameter item
// This opens a dialog box for a user-defined item, and writes
// the value to a file to be read next time.
// ==============================================================

///
/// \ingroup Config
///
class ProjectApolloConfigurator: public LaunchpadItem {
public:
	ProjectApolloConfigurator();
	char *Name() { return "Project Apollo Configuration"; }
	char *Description() { return "Global configuration for Project Apollo - NASSP."; }
	bool clbkOpen (HWND hLaunchpad);
	int clbkWriteConfig ();
	static BOOL CALLBACK DlgProcFrame (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK DlgProcVisual (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK DlgProcControl (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	void WriteConfig(FILEHANDLE hFile);
	static void UpdateControlState(HWND hWnd);
};

ProjectApolloConfigurator::ProjectApolloConfigurator (): LaunchpadItem ()
{
	char *line;
	// Read the current parameter values from file
	FILEHANDLE hFile = oapiOpenFile(cfgfile, FILE_IN, CONFIG);
	while (oapiReadScenario_nextline(hFile, line)) {
		if (!strnicmp (line, "MAINPANELSPLIT", 14)) {
			sscanf (line + 14, "%i", &gParams.Saturn_MainPanelSplit);
		} else if (!strnicmp (line, "LOWRES", 6)) {
			sscanf (line + 6, "%i", &gParams.Saturn_LowRes);
		} else if (!strnicmp (line, "FDAIDISABLED", 12)) {
			sscanf (line + 12, "%i", &gParams.Saturn_FDAIDisabled);
		} else if (!strnicmp (line, "FDAISMOOTH", 10)) {
			sscanf (line + 10, "%i", &gParams.Saturn_FDAISmooth);
		} else if (!strnicmp (line, "GNSPLIT", 7)) {
			sscanf (line + 7, "%i", &gParams.Saturn_GNSplit);
		} else if (!strnicmp (line, "MAXTIMEACC", 10)) {
			sscanf (line + 10, "%i", &gParams.Saturn_MaxTimeAcceleration);
		} else if (!strnicmp (line, "MULTITHREAD", 11)) {
			sscanf (line + 11, "%i", &gParams.Saturn_MultiThread);
		} else if (!strnicmp (line, "VAGCCHECKLISTAUTOSLOW", 21)) {
			sscanf (line + 21, "%i", &gParams.Saturn_VAGCChecklistAutoSlow);
		} else if (!strnicmp (line, "VAGCCHECKLISTAUTOENABLED", 24)) {
			sscanf (line + 24, "%i", &gParams.Saturn_VAGCChecklistAutoEnabled);
		} else if (!strnicmp (line, "JOYSTICK_RHC", 12)) {
			sscanf (line + 12, "%i", &gParams.Saturn_RHC);
		} else if (!strnicmp (line, "JOYSTICK_THC", 12)) {
			sscanf (line + 12, "%i", &gParams.Saturn_THC);
		} else if (!strnicmp(line, "JOYSTICK_RSL", 12)) {
			sscanf(line + 12, "%i", &gParams.Saturn_RSL);
		} else if (!strnicmp(line, "JOYSTICK_TSL", 12)) {
			sscanf(line + 12, "%i", &gParams.Saturn_TSL);
		} else if (!strnicmp (line, "JOYSTICK_RTTID", 14)) {
			sscanf (line + 14, "%i", &gParams.Saturn_RHCTHCToggleId);
		} else if (!strnicmp (line, "JOYSTICK_RTT", 12)) {
			sscanf (line + 12, "%i", &gParams.Saturn_RHCTHCToggle);
		} else if (!strnicmp(line, "VCINFOENABLED", 13)) {
			sscanf(line + 13, "%i", &gParams.Saturn_VcInfoEnabled);
		}
	}	
	oapiCloseFile (hFile, FILE_IN);
}

bool ProjectApolloConfigurator::clbkOpen (HWND hLaunchpad)
{
	// respond to user double-clicking the item in the list
	DialogBox(gParams.hInst, MAKEINTRESOURCE (IDD_MYFRAME), hLaunchpad, DlgProcFrame);
	return true;
}

int ProjectApolloConfigurator::clbkWriteConfig ()
{
	// called when orbiter needs to write its configuration to disk
	WriteConfig(oapiOpenFile(cfgfile, FILE_OUT, CONFIG));
	WriteConfig(oapiOpenFile("ProjectApollo/Saturn1b.launchpad.cfg", FILE_OUT, CONFIG));
	WriteConfig(oapiOpenFile("ProjectApollo/LEM.launchpad.cfg", FILE_OUT, CONFIG));

	return 0;
}

void ProjectApolloConfigurator::WriteConfig(FILEHANDLE hFile)
{
	char cbuf[1000];

	sprintf(cbuf, "LOWRES %d", gParams.Saturn_LowRes);
	oapiWriteLine(hFile, cbuf);

	sprintf(cbuf, "MAINPANELSPLIT %d", gParams.Saturn_MainPanelSplit);
	oapiWriteLine(hFile, cbuf);

	sprintf(cbuf, "GNSPLIT %d", gParams.Saturn_GNSplit);
	oapiWriteLine(hFile, cbuf);

	sprintf(cbuf, "FDAIDISABLED %d", gParams.Saturn_FDAIDisabled);
	oapiWriteLine(hFile, cbuf);

	sprintf(cbuf, "FDAISMOOTH %d", gParams.Saturn_FDAISmooth);
	oapiWriteLine(hFile, cbuf);

	sprintf(cbuf, "MAXTIMEACC %d", gParams.Saturn_MaxTimeAcceleration);
	oapiWriteLine(hFile, cbuf);

	sprintf(cbuf, "MULTITHREAD %d", gParams.Saturn_MultiThread);
	oapiWriteLine(hFile, cbuf);

	sprintf(cbuf, "VAGCCHECKLISTAUTOSLOW %d", gParams.Saturn_VAGCChecklistAutoSlow);
	oapiWriteLine(hFile, cbuf);

	sprintf(cbuf, "VAGCCHECKLISTAUTOENABLED %d", gParams.Saturn_VAGCChecklistAutoEnabled);
	oapiWriteLine(hFile, cbuf);

	sprintf(cbuf, "JOYSTICK_RHC %d", gParams.Saturn_RHC);
	oapiWriteLine(hFile, cbuf);

	oapiWriteLine(hFile, "JOYSTICK_RAUTO");	// Not configurable currently

	sprintf(cbuf, "JOYSTICK_RTT %d", gParams.Saturn_RHCTHCToggle);
	oapiWriteLine(hFile, cbuf);
	
	sprintf(cbuf, "JOYSTICK_RTTID %d", gParams.Saturn_RHCTHCToggleId);
	oapiWriteLine(hFile, cbuf);
	
	sprintf(cbuf, "JOYSTICK_THC %d", gParams.Saturn_THC);
	oapiWriteLine(hFile, cbuf);

	sprintf(cbuf, "JOYSTICK_RSL %d", gParams.Saturn_RSL);
	oapiWriteLine(hFile, cbuf);

	sprintf(cbuf, "JOYSTICK_TSL %d", gParams.Saturn_TSL);
	oapiWriteLine(hFile, cbuf);

	oapiWriteLine(hFile, "JOYSTICK_TAUTO");	// Not configurable currently

	sprintf(cbuf, "VCINFOENABLED %d", gParams.Saturn_VcInfoEnabled);
	oapiWriteLine(hFile, cbuf);

	oapiCloseFile (hFile, FILE_OUT);
}

BOOL CALLBACK ProjectApolloConfigurator::DlgProcFrame (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	TCITEM tabitem;
    HWND hTab;
    NMHDR *hdr;
	POINT pt;
	RECT rc;
	char buffer[100];
	int i;

	switch (uMsg) {
	case WM_INITDIALOG: // display the current value

	    hTab = GetDlgItem(hWnd, IDC_TAB); 
		memset(&tabitem, 0, sizeof(tabitem));	
    
		tabitem.mask = TCIF_TEXT;
		tabitem.pszText = "Visuals";
		SendMessage(hTab, TCM_INSERTITEM, 0, (LPARAM) &tabitem);

		tabitem.pszText = "Controls";
		SendMessage(hTab, TCM_INSERTITEM, 1, (LPARAM) &tabitem);

		tabitem.pszText = "Miscellaneous";
		SendMessage(hTab, TCM_INSERTITEM, 2, (LPARAM) &tabitem);

		// set tab control display area
		GetWindowRect(hTab, &rc);
		TabCtrl_AdjustRect(hTab, false, &rc);
		pt.x = rc.left;
		pt.y = rc.top;
		ScreenToClient(hWnd, &pt);

		// create the child windows
		gParams.hDlgTabs[0] = CreateDialog(gParams.hInst, MAKEINTRESOURCE(IDD_PAGEVISUAL), hWnd, (DLGPROC) DlgProcVisual);
		MoveWindow(gParams.hDlgTabs[0], pt.x, pt.y, rc.right - rc.left, rc.bottom - rc.top, false);
		ShowWindow(gParams.hDlgTabs[0], SW_SHOW);

		gParams.hDlgTabs[1] = CreateDialog(gParams.hInst, MAKEINTRESOURCE(IDD_PAGECONTROL), hWnd, (DLGPROC) DlgProcControl);
		MoveWindow(gParams.hDlgTabs[1], pt.x, pt.y, rc.right - rc.left, rc.bottom - rc.top, false);
		ShowWindow(gParams.hDlgTabs[1], SW_HIDE);

		gParams.hDlgTabs[2] = CreateDialog(gParams.hInst, MAKEINTRESOURCE(IDD_PAGEMISC), hWnd, (DLGPROC) DlgProcControl);
		MoveWindow(gParams.hDlgTabs[2], pt.x, pt.y, rc.right - rc.left, rc.bottom - rc.top, false);
		ShowWindow(gParams.hDlgTabs[2], SW_HIDE);

		return TRUE;

	case WM_NOTIFY:
	     hdr = (LPNMHDR) lParam;
	     if (hdr->code == TCN_SELCHANGING || hdr->code == TCN_SELCHANGE) {
		   int index;
		   index = TabCtrl_GetCurSel(hdr->hwndFrom);
		   if (index >= 0 && index < MAX_TABNUM) 
			   ShowWindow(gParams.hDlgTabs[index], (hdr->code == TCN_SELCHANGE) ? SW_SHOW : SW_HIDE);
	     }
	     break;
 
	case WM_COMMAND:
			switch (LOWORD (wParam)) {
			case IDOK:    // store the value
				// Visuals Tab
				if (SendDlgItemMessage (gParams.hDlgTabs[0], IDC_RADIO_HIGHRES, BM_GETCHECK, 0, 0) == BST_UNCHECKED) {
					gParams.Saturn_LowRes = 1;
				} else {
					gParams.Saturn_LowRes = 0;
				}
				if (SendDlgItemMessage (gParams.hDlgTabs[0], IDC_RADIO_SINGLEPANEL, BM_GETCHECK, 0, 0) == BST_UNCHECKED) {
					gParams.Saturn_MainPanelSplit = 1;
				} else {
					gParams.Saturn_MainPanelSplit = 0;
				}
				if (SendDlgItemMessage (gParams.hDlgTabs[0], IDC_RADIO_GNSINGLEPANEL, BM_GETCHECK, 0, 0) == BST_UNCHECKED) {
					gParams.Saturn_GNSplit = 1;
				} else {
					gParams.Saturn_GNSplit = 0;
				}
				if (SendDlgItemMessage (gParams.hDlgTabs[0], IDC_CHECK_FDAIDISABLED, BM_GETCHECK, 0, 0) == BST_CHECKED) {
					gParams.Saturn_FDAIDisabled = 1;
				} else {
					gParams.Saturn_FDAIDisabled = 0;
				}
				if (SendDlgItemMessage (gParams.hDlgTabs[0], IDC_CHECK_FDAISMOOTH, BM_GETCHECK, 0, 0) == BST_CHECKED) {
					gParams.Saturn_FDAISmooth = 1;
				} else {
					gParams.Saturn_FDAISmooth = 0;
				}

				// Controls Tab
				if (SendDlgItemMessage (gParams.hDlgTabs[1], IDC_CHECK_RHC, BM_GETCHECK, 0, 0) == BST_UNCHECKED) {
					gParams.Saturn_RHC = -1;
				} else {
					SendDlgItemMessage(gParams.hDlgTabs[1], IDC_EDIT_RHC, WM_GETTEXT, 3, (LPARAM) (LPCTSTR) buffer);
					if (sscanf(buffer, "%i", &i) == 1) {
						gParams.Saturn_RHC = i;
					} else {
						gParams.Saturn_RHC = -1;
					}
				}
				if (SendDlgItemMessage (gParams.hDlgTabs[1], IDC_CHECK_THC, BM_GETCHECK, 0, 0) == BST_UNCHECKED) {
					gParams.Saturn_THC = -1;
				} else {
					SendDlgItemMessage(gParams.hDlgTabs[1], IDC_EDIT_THC, WM_GETTEXT, 3, (LPARAM) (LPCTSTR) buffer);
					if (sscanf(buffer, "%i", &i) == 1) {
						gParams.Saturn_THC = i;
					} else {
						gParams.Saturn_THC = -1;
					}
				}
				if (SendDlgItemMessage (gParams.hDlgTabs[1], IDC_CHECK_RHCTHCTOGGLE, BM_GETCHECK, 0, 0) == BST_UNCHECKED) {
					gParams.Saturn_RHCTHCToggle = 0;
				} else {
					gParams.Saturn_RHCTHCToggle = 1;
				}

				SendDlgItemMessage(gParams.hDlgTabs[1], IDC_EDIT_RHCTHCTOGGLE, WM_GETTEXT, 3, (LPARAM) (LPCTSTR) buffer);
				if (sscanf(buffer, "%i", &i) == 1) {
					gParams.Saturn_RHCTHCToggleId = i;
				} else {
					gParams.Saturn_RHCTHCToggleId = -1;
				}

				if (SendDlgItemMessage(gParams.hDlgTabs[1], IDC_CHECK_RSL, BM_GETCHECK, 0, 0) == BST_UNCHECKED) {
					gParams.Saturn_RSL = -1;
				}
				else {
					SendDlgItemMessage(gParams.hDlgTabs[1], IDC_EDIT_RSL, WM_GETTEXT, 3, (LPARAM)(LPCTSTR)buffer);
					if (sscanf(buffer, "%i", &i) == 1) {
						gParams.Saturn_RSL = i;
					}
					else {
						gParams.Saturn_RSL = -1;
					}
				}

				if (SendDlgItemMessage(gParams.hDlgTabs[1], IDC_CHECK_TJT, BM_GETCHECK, 0, 0) == BST_UNCHECKED) {
					gParams.Saturn_TSL = -1;
				}
				else {
					SendDlgItemMessage(gParams.hDlgTabs[1], IDC_EDIT_TJT, WM_GETTEXT, 3, (LPARAM)(LPCTSTR)buffer);
					if (sscanf(buffer, "%i", &i) == 1) {
						gParams.Saturn_TSL = i;
					}
					else {
						gParams.Saturn_TSL = -1;
					}
				}

				// Miscellaneous Tab
				SendDlgItemMessage(gParams.hDlgTabs[2], IDC_EDIT_TIMEACC, WM_GETTEXT, 4, (LPARAM) (LPCTSTR) buffer);
				if (sscanf(buffer, "%i", &i) == 1) {
					gParams.Saturn_MaxTimeAcceleration = i;
				} else {
					gParams.Saturn_MaxTimeAcceleration = 100;
				}
				if (SendDlgItemMessage (gParams.hDlgTabs[2], IDC_CHECK_MULTITHREAD, BM_GETCHECK, 0, 0) == BST_CHECKED) {
					gParams.Saturn_MultiThread = 1;
				} else {
					gParams.Saturn_MultiThread = 0;
				}
				if (SendDlgItemMessage (gParams.hDlgTabs[2], IDC_CHECK_VAGCCHECKLISTAUTOSLOW, BM_GETCHECK, 0, 0) == BST_CHECKED) {
					gParams.Saturn_VAGCChecklistAutoSlow = 1;
				} else {
					gParams.Saturn_VAGCChecklistAutoSlow = 0;
				}
				if (SendDlgItemMessage (gParams.hDlgTabs[2], IDC_CHECK_VAGCCHECKLISTAUTOENABLED, BM_GETCHECK, 0, 0) == BST_CHECKED) {
					gParams.Saturn_VAGCChecklistAutoEnabled = 1;
				} else {
					gParams.Saturn_VAGCChecklistAutoEnabled = 0;
				}
				if (SendDlgItemMessage(gParams.hDlgTabs[2], IDC_CHECK_VCINFOENABLED, BM_GETCHECK, 0, 0) == BST_CHECKED) {
					gParams.Saturn_VcInfoEnabled = 1;
				}
				else {
					gParams.Saturn_VcInfoEnabled = 0;
				}

				EndDialog (hWnd, 0);
				return 0;

			case IDCANCEL:
				EndDialog (hWnd, 0);
				return 0;
			}
		
		break;
	}
	return 0;
}

BOOL CALLBACK ProjectApolloConfigurator::DlgProcVisual (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_INITDIALOG: // display the current value
		SendDlgItemMessage(hWnd, IDC_RADIO_HIGHRES,   BM_SETCHECK, gParams.Saturn_LowRes?BST_UNCHECKED:BST_CHECKED, 0);
		SendDlgItemMessage(hWnd, IDC_RADIO_LOWRES, BM_SETCHECK, gParams.Saturn_LowRes?BST_CHECKED:BST_UNCHECKED, 0);

		SendDlgItemMessage(hWnd, IDC_RADIO_SINGLEPANEL,   BM_SETCHECK, gParams.Saturn_MainPanelSplit?BST_UNCHECKED:BST_CHECKED, 0);
		SendDlgItemMessage(hWnd, IDC_RADIO_SPLITTEDPANEL, BM_SETCHECK, gParams.Saturn_MainPanelSplit?BST_CHECKED:BST_UNCHECKED, 0);

		SendDlgItemMessage(hWnd, IDC_RADIO_GNSINGLEPANEL, BM_SETCHECK, gParams.Saturn_GNSplit?BST_UNCHECKED:BST_CHECKED, 0);
		SendDlgItemMessage(hWnd, IDC_RADIO_GNSPLITTEDPANEL, BM_SETCHECK, gParams.Saturn_GNSplit?BST_CHECKED:BST_UNCHECKED, 0);

		SendDlgItemMessage(hWnd, IDC_CHECK_FDAIDISABLED, BM_SETCHECK, gParams.Saturn_FDAIDisabled?BST_CHECKED:BST_UNCHECKED, 0);
		SendDlgItemMessage(hWnd, IDC_CHECK_FDAISMOOTH, BM_SETCHECK, gParams.Saturn_FDAISmooth?BST_CHECKED:BST_UNCHECKED, 0);
		
		return TRUE;
	}
	return 0;
}

BOOL CALLBACK ProjectApolloConfigurator::DlgProcControl (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	char buffer[100];

	switch (uMsg) {
	case WM_INITDIALOG: // display the current value
		if (gParams.Saturn_RHC == -1) {
			SendDlgItemMessage(hWnd, IDC_CHECK_RHC, BM_SETCHECK, BST_UNCHECKED, 0);
			SendDlgItemMessage(hWnd, IDC_EDIT_RHC, WM_SETTEXT, 0, (LPARAM) (LPCTSTR) "0");
		} else {
			SendDlgItemMessage(hWnd, IDC_CHECK_RHC, BM_SETCHECK, BST_CHECKED, 0);
			sprintf(buffer, "%i", gParams.Saturn_RHC);
			SendDlgItemMessage(hWnd, IDC_EDIT_RHC, WM_SETTEXT, 0, (LPARAM) (LPCTSTR) buffer);
		}

		if (gParams.Saturn_THC == -1) {
			SendDlgItemMessage(hWnd, IDC_CHECK_THC, BM_SETCHECK, BST_UNCHECKED, 0);
			SendDlgItemMessage(hWnd, IDC_EDIT_THC, WM_SETTEXT, 0, (LPARAM) (LPCTSTR) "1");
		} else {
			SendDlgItemMessage(hWnd, IDC_CHECK_THC, BM_SETCHECK, BST_CHECKED, 0);
			sprintf(buffer, "%i", gParams.Saturn_THC);
			SendDlgItemMessage(hWnd, IDC_EDIT_THC, WM_SETTEXT, 0, (LPARAM) (LPCTSTR) buffer);
		}

		SendDlgItemMessage(hWnd, IDC_CHECK_RHCTHCTOGGLE, BM_SETCHECK, gParams.Saturn_RHCTHCToggle?BST_CHECKED:BST_UNCHECKED, 0);

		if (gParams.Saturn_RHCTHCToggleId == -1) {
			SendDlgItemMessage(hWnd, IDC_EDIT_RHCTHCTOGGLE, WM_SETTEXT, 0, (LPARAM) (LPCTSTR) "0");
		} else {
			sprintf(buffer, "%i", gParams.Saturn_RHCTHCToggleId);
			SendDlgItemMessage(hWnd, IDC_EDIT_RHCTHCTOGGLE, WM_SETTEXT, 0, (LPARAM) (LPCTSTR) buffer);
		}

		if (gParams.Saturn_RSL == -1) {
			SendDlgItemMessage(hWnd, IDC_CHECK_RSL, BM_SETCHECK, BST_UNCHECKED, 0);
			SendDlgItemMessage(hWnd, IDC_EDIT_RSL, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) "0");
		}
		else {
			SendDlgItemMessage(hWnd, IDC_CHECK_RSL, BM_SETCHECK, BST_CHECKED, 0);
			sprintf(buffer, "%i", gParams.Saturn_RSL);
			SendDlgItemMessage(hWnd, IDC_EDIT_RSL, WM_SETTEXT, 0, (LPARAM)(LPCTSTR)buffer);
		}

		if (gParams.Saturn_TSL == -1) {
			SendDlgItemMessage(hWnd, IDC_CHECK_TJT, BM_SETCHECK, BST_UNCHECKED, 0);
			SendDlgItemMessage(hWnd, IDC_EDIT_TJT, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) "1");
		}
		else {
			SendDlgItemMessage(hWnd, IDC_CHECK_TJT, BM_SETCHECK, BST_CHECKED, 0);
			sprintf(buffer, "%i", gParams.Saturn_TSL);
			SendDlgItemMessage(hWnd, IDC_EDIT_TJT, WM_SETTEXT, 0, (LPARAM)(LPCTSTR)buffer);
		}

		sprintf(buffer,"%i",gParams.Saturn_MaxTimeAcceleration);
		SendDlgItemMessage(hWnd, IDC_EDIT_TIMEACC, WM_SETTEXT, 0, (LPARAM) (LPCTSTR) buffer);

		SendDlgItemMessage(hWnd, IDC_CHECK_MULTITHREAD, BM_SETCHECK, gParams.Saturn_MultiThread?BST_CHECKED:BST_UNCHECKED, 0);

		SendDlgItemMessage(hWnd, IDC_CHECK_VAGCCHECKLISTAUTOSLOW, BM_SETCHECK, gParams.Saturn_VAGCChecklistAutoSlow?BST_CHECKED:BST_UNCHECKED, 0);

		SendDlgItemMessage(hWnd, IDC_CHECK_VAGCCHECKLISTAUTOENABLED, BM_SETCHECK, gParams.Saturn_VAGCChecklistAutoEnabled?BST_CHECKED:BST_UNCHECKED, 0);

		SendDlgItemMessage(hWnd, IDC_CHECK_VCINFOENABLED, BM_SETCHECK, gParams.Saturn_VcInfoEnabled ? BST_CHECKED : BST_UNCHECKED, 0);

		UpdateControlState(hWnd);
		return TRUE;

	case WM_COMMAND:
		if (HIWORD(wParam) == BN_CLICKED && (HWND)lParam == GetDlgItem(hWnd, IDC_CHECK_RHC)) {
			UpdateControlState(hWnd);

		} else if (HIWORD(wParam) == BN_CLICKED && (HWND)lParam == GetDlgItem(hWnd, IDC_CHECK_THC)) {
			UpdateControlState(hWnd);

		} else if (HIWORD(wParam) == BN_CLICKED && (HWND)lParam == GetDlgItem(hWnd, IDC_CHECK_RHCTHCTOGGLE)) {
			UpdateControlState(hWnd);

		} else if (HIWORD(wParam) == BN_CLICKED && (HWND)lParam == GetDlgItem(hWnd, IDC_CHECK_RSL)) {
			UpdateControlState(hWnd);

		} else if (HIWORD(wParam) == BN_CLICKED && (HWND)lParam == GetDlgItem(hWnd, IDC_CHECK_TJT)) {
			UpdateControlState(hWnd);
		}
		break;

	case WM_CTLCOLORSTATIC:
		// Set the color of the text
		if ((HWND)lParam == GetDlgItem(hWnd, IDC_STATIC_JOYATT)) {
			// were about to draw the static
			// set the text color in (HDC)lParam
			SetBkMode((HDC)wParam,TRANSPARENT);
			SetTextColor((HDC)wParam, RGB(255,0,0));
			return (BOOL)CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
		}
		break;
	}
	return 0;
}

void ProjectApolloConfigurator::UpdateControlState(HWND hWnd) {

	long rhcChecked, thcChecked;

	rhcChecked = SendDlgItemMessage (hWnd, IDC_CHECK_RHC, BM_GETCHECK, 0, 0);
	thcChecked = SendDlgItemMessage (hWnd, IDC_CHECK_THC, BM_GETCHECK, 0, 0);

	if (rhcChecked == BST_CHECKED) {
		SendDlgItemMessage(hWnd, IDC_EDIT_RHC, EM_SETREADONLY, (WPARAM) (BOOL) false, 0);
		EnableWindow(GetDlgItem(hWnd, IDC_STATIC_RHC), TRUE);
	} else {
		SendDlgItemMessage(hWnd, IDC_EDIT_RHC, EM_SETREADONLY, (WPARAM) (BOOL) true, 0);
		EnableWindow(GetDlgItem(hWnd, IDC_STATIC_RHC), FALSE);
	}

	if (thcChecked == BST_CHECKED) {
		SendDlgItemMessage(hWnd, IDC_EDIT_THC, EM_SETREADONLY, (WPARAM) (BOOL) false, 0);
		EnableWindow(GetDlgItem(hWnd, IDC_STATIC_THC), TRUE);
	} else {
		SendDlgItemMessage(hWnd, IDC_EDIT_THC, EM_SETREADONLY, (WPARAM) (BOOL) true, 0);
		EnableWindow(GetDlgItem(hWnd, IDC_STATIC_THC), FALSE);
	}

	if (rhcChecked == BST_CHECKED && thcChecked == BST_UNCHECKED) {
		EnableWindow(GetDlgItem(hWnd, IDC_CHECK_RHCTHCTOGGLE), TRUE);
	} else {
		EnableWindow(GetDlgItem(hWnd, IDC_CHECK_RHCTHCTOGGLE), FALSE);
		SendDlgItemMessage(hWnd, IDC_CHECK_RHCTHCTOGGLE, BM_SETCHECK, BST_UNCHECKED, 0);
	}

	if (SendDlgItemMessage (hWnd, IDC_CHECK_RHCTHCTOGGLE, BM_GETCHECK, 0, 0) == BST_CHECKED) {
		SendDlgItemMessage(hWnd, IDC_EDIT_RHCTHCTOGGLE, EM_SETREADONLY, (WPARAM) (BOOL) false, 0);
		EnableWindow(GetDlgItem(hWnd, IDC_STATIC_RHCTHCTOGGLE), TRUE);
	} else {
		SendDlgItemMessage(hWnd, IDC_EDIT_RHCTHCTOGGLE, EM_SETREADONLY, (WPARAM) (BOOL) true, 0);
		EnableWindow(GetDlgItem(hWnd, IDC_STATIC_RHCTHCTOGGLE), FALSE);
	}

	if (rhcChecked == BST_CHECKED) {
		EnableWindow(GetDlgItem(hWnd, IDC_CHECK_RSL), TRUE);
	}
	else {
		EnableWindow(GetDlgItem(hWnd, IDC_CHECK_RSL), FALSE);
		SendDlgItemMessage(hWnd, IDC_CHECK_RSL, BM_SETCHECK, BST_UNCHECKED, 0);
	}

	if (SendDlgItemMessage(hWnd, IDC_CHECK_RSL, BM_GETCHECK, 0, 0) == BST_CHECKED) {
		SendDlgItemMessage(hWnd, IDC_EDIT_RSL, EM_SETREADONLY, (WPARAM)(BOOL)false, 0);
		EnableWindow(GetDlgItem(hWnd, IDC_STATIC_RSL), TRUE);
	}
	else {
		SendDlgItemMessage(hWnd, IDC_EDIT_RSL, EM_SETREADONLY, (WPARAM)(BOOL)true, 0);
		EnableWindow(GetDlgItem(hWnd, IDC_STATIC_RSL), FALSE);
	}

	if (thcChecked == BST_CHECKED) {
		EnableWindow(GetDlgItem(hWnd, IDC_CHECK_TJT), TRUE);
	}
	else {
		EnableWindow(GetDlgItem(hWnd, IDC_CHECK_TJT), FALSE);
		SendDlgItemMessage(hWnd, IDC_CHECK_TJT, BM_SETCHECK, BST_UNCHECKED, 0);
	}

	if (SendDlgItemMessage(hWnd, IDC_CHECK_TJT, BM_GETCHECK, 0, 0) == BST_CHECKED) {
		SendDlgItemMessage(hWnd, IDC_EDIT_TJT, EM_SETREADONLY, (WPARAM)(BOOL)false, 0);
		EnableWindow(GetDlgItem(hWnd, IDC_STATIC_TJT), TRUE);
	}
	else {
		SendDlgItemMessage(hWnd, IDC_EDIT_TJT, EM_SETREADONLY, (WPARAM)(BOOL)true, 0);
		EnableWindow(GetDlgItem(hWnd, IDC_STATIC_TJT), FALSE);
	}
}

// ==============================================================
// The DLL entry point
// ==============================================================

DLLCLBK void opcDLLInit (HINSTANCE hDLL)
{
	int i;

	// create the new config item
	gParams.hInst = hDLL;

	gParams.Saturn_LowRes = 0;
	gParams.Saturn_MainPanelSplit = 0;
	gParams.Saturn_GNSplit = 1;
	gParams.Saturn_FDAIDisabled = 0;
	gParams.Saturn_FDAISmooth = 0;
	gParams.Saturn_RHC = -1;
	gParams.Saturn_THC = -1;
	gParams.Saturn_RSL = -1;
	gParams.Saturn_TSL = -1;
	gParams.Saturn_RHCTHCToggle = 0;
	gParams.Saturn_RHCTHCToggleId = -1;
	gParams.Saturn_MaxTimeAcceleration = 0;
	gParams.Saturn_MultiThread = 0;
	gParams.Saturn_VAGCChecklistAutoSlow = 1;
	gParams.Saturn_VAGCChecklistAutoEnabled = 0;
	gParams.Saturn_VcInfoEnabled = 0;

	gParams.item = new ProjectApolloConfigurator;
	for (i = 0; i < MAX_TABNUM; i++)
		gParams.hDlgTabs[i] = NULL;

	// find the config root entry provided by orbiter
	LAUNCHPADITEM_HANDLE root = oapiFindLaunchpadItem ("Vessel configuration");
	// register the config entry
	oapiRegisterLaunchpadItem (gParams.item, root);
}

// ==============================================================
// The DLL exit point
// ==============================================================

DLLCLBK void opcDLLExit (HINSTANCE hDLL)
{
	// Unregister the launchpad items
	oapiUnregisterLaunchpadItem (gParams.item);
	delete gParams.item;
}

// 0 = 4:3
// 1 = 16:10
// 2 = 16:9
static int renderViewportIsWideScreen = 0;

DLLCLBK void opcOpenRenderViewport(HWND renderWnd, DWORD width, DWORD height, BOOL fullscreen)

{
	if (((double) width) / ((double) height) < 1.47) 
		renderViewportIsWideScreen = 0;
	else if (((double) width) / ((double) height) < 1.69) 
		renderViewportIsWideScreen = 1;
	else
		renderViewportIsWideScreen = 2;
}

DLLCLBK int pacRenderViewportIsWideScreen() 
{
	return renderViewportIsWideScreen;
}

static SOCKET close_Socket = INVALID_SOCKET;
static SOCKET close_Socket_LM = INVALID_SOCKET;

DLLCLBK bool pacDefineSocket(SOCKET sockettoclose)
{
	close_Socket = sockettoclose;
	HMODULE hpamfd = GetModuleHandle("modules//plugin//ProjectApolloMFD.dll");
	if (hpamfd)	{
		bool (__cdecl *defineSocketext)(SOCKET);
		defineSocketext = (bool (_cdecl *)(SOCKET))GetProcAddress(hpamfd, "pacDefineSocket");
		if (defineSocketext) {
			defineSocketext(sockettoclose);
		}
	}
	return true;
}

DLLCLBK bool pacDefineSocketLM(SOCKET sockettoclose)
{
	close_Socket_LM = sockettoclose;
	HMODULE hpamfd = GetModuleHandle("modules//plugin//ProjectApolloMFD.dll");
	if (hpamfd)	{
		bool (__cdecl *defineSocketext)(SOCKET);
		defineSocketext = (bool (_cdecl *)(SOCKET))GetProcAddress(hpamfd, "pacDefineSocketLM");
		if (defineSocketext) {
			defineSocketext(sockettoclose);
		}
	}
	return true;
}

DLLCLBK void opcCloseRenderViewport()
{
	if (close_Socket != INVALID_SOCKET)	{
		shutdown(close_Socket, 2);	//Shutdown, NOW!!!
		closesocket(close_Socket);
	}
	if (close_Socket_LM != INVALID_SOCKET)	{
		shutdown(close_Socket_LM, 2);	// LM socket too
		closesocket(close_Socket_LM);
	}
}
