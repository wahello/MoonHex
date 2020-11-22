#pragma once

#include <chrono>

#include <wx/file.h>
#include <wx/dcbuffer.h>
#include <wx/vscroll.h>
#include <wx/filename.h>
#include <wx/timer.h>

#include <moon/table.hpp>
#include <moon/bit_conv.hpp>

#ifdef _DEBUG
#define SEPARATOR_COLOR 197,0,0
#else 
#define SEPARATOR_COLOR 200,200,200
#endif

class wxHexEvent : public wxEvent
{	
public:
	wxHexEvent(wxEventType hexType = wxEVT_NULL) :
		wxEvent(hexType)
		{

		}		

	wxHexEvent(wxWindowID winId, wxEventType hexType = wxEVT_NULL) :
		wxEvent(winId, hexType)
		{

		}

	wxHexEvent(const wxHexEvent& event) :
		wxEvent(event)
		{
			SetOffset(event.GetOffset());
		}

	wxEvent* Clone() const 
	{
		return new wxHexEvent(*this);
	}
private:
	uint32_t m_Offset;
public:
	uint32_t GetOffset() const { return m_Offset; }

	void SetOffset(uint32_t offset) { m_Offset = offset; }
};

typedef void (wxEvtHandler::*wxHexEventFunction)(wxHexEvent& event);

class wxHexCtrl : public wxHVScrolledWindow
{
public:
	wxHexCtrl(wxWindow* parent, wxWindowID id);
	~wxHexCtrl() = default;
public:
	void OpenFile(const wxString& path);
	void OpenTable(const wxString& path);	
	void SetOffset(size_t offset, bool scroll = false);	
	void SetLastOffsetChange();	
	size_t GetOffset();
	size_t GetFileSize() { return m_File.Length(); }
	size_t GetPageByteCount() { return m_Col*GetRowCount(); }
private:
	wxFile m_File;
	uint8_t* m_Data = nullptr;
	size_t m_Col = 16;
	size_t m_Rows;
	wxTimer m_UpdateSel;	
//Sub windows stuff
private:	
	wxRect m_ColWindowRect;
	wxRect m_OffsetWindowRect;
	wxRect m_ByteWindowRect;
	wxRect m_CharWindowRect;

	wxPoint GetBytePosition(const size_t& offset, const bool n = 0);
	wxPoint GetCharPosition(const size_t& offset);
//Caret	
private:
	bool m_SelectedByte = true;
	bool m_RightNibble = false;
	bool m_DrawCaret = true;
	size_t m_Offset = 0;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_LastOffsetChange;
	wxColour m_CaretSelBg[2] = { {192, 192, 192}, {0, 0, 255} };	
	wxColour m_CaretSelFore[2] = { {0, 0, 0}, {255, 255, 0} };	
//Selection
private:
	uint32_t m_SelStart = 0;
	uint32_t m_SelLength = 0;
	wxColour m_SelPenColour = { 20, 205, 196 };
	wxColour m_SelBrushColour = { 202, 245, 246 };
	bool IsSquareSelection();
public:
	size_t GetSelStart() const { return m_SelStart; }
	size_t GetSelEnd() const { return m_SelStart+m_SelLength-1; }	
	wxPosition GetSelPosition() const;
	wxPosition GetSelEndPosition() const;
//Table	
private:
	Moon::Hacking::Table m_Table;	
private:
	wxSize m_CharSize;
//Overrides
private:
	virtual wxCoord OnGetRowHeight(size_t row) const;
	virtual wxCoord OnGetColumnWidth(size_t col) const;
//Drawing
private:	
	void CalculateMinSize();
	void OnDraw(wxDC& dc);
	void DrawLines(wxDC& dc);	
	void DrawSeparator(wxDC& dc, wxPoint start, wxPoint end);
	void DrawBytePage(wxDC& dc);
	void DrawCharPage(wxDC& dc);
	void DrawCols(wxDC& dc);
	void DrawOffsets(wxDC& dc);	
	void DrawCaret(wxDC& dc);
	void DrawRect(wxDC& dc, const wxRect& r, const wxColour& c);
	void DrawByte(wxDC& dc, const uint8_t& b, const wxPoint& p, const wxColour& c);
	bool CanDrawChar(const char& _c);
	void DrawSelection(wxDC& dc);
	size_t GetLastDrawingLine();
	//void DrawSelection(wxDC& dc);
//Events
private:
	void OnPaintEvent(wxPaintEvent& event);
	void OnLeftDown(wxMouseEvent& event);
	void OnMouseMove(wxMouseEvent& event);
	void OnSelectionTimer(wxTimerEvent& event);
	void OnResize(wxSizeEvent& event);	
	void OnKeyDown(wxKeyEvent& event);
	void OnChar(wxKeyEvent& event);
};
