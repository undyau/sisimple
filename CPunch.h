// Class automatically generated by Dev-C++ New Class wizard

#ifndef CPUNCH_H
#define CPUNCH_H
#include <wx/datetime.h>
#include <wx/string.h>

// Represents a punch at a control
class CPunch
{
	public:
		// class constructor
		CPunch(int a_CN, wxString a_DOW, wxDateTime a_When);
		CPunch();
		CPunch(const CPunch& a_Copy);
		CPunch& operator=(const CPunch& a_Other);
        bool operator<(const CPunch& a_Other) const;
        bool operator==(const CPunch& a_Other) const;        
        bool operator!=(const CPunch& a_Other) const {return !operator==(a_Other);};         
		
		void SetData(long a_CN, wxString a_DOW, wxDateTime a_When);
		
		// class destructor
		~CPunch();
		void SetCN(long x); // sets the value of m_CN
		long GetCN(); // returns the value of m_CN
		void SetDOW(wxString x); // sets the value of m_DOW
		wxString GetDOW(); // returns the value of m_DOW
		void SetWhen(wxDateTime x); // sets the value of m_When
		wxDateTime GetWhen(); // returns the value of m_When
		void SetIgnore(bool a_Ignore) {m_Ignore = a_Ignore;};
		bool GetIgnore() {return m_Ignore;};		
		
	private:
		// Control Number
		long m_CN;
		// Day of week
		wxString m_DOW;
		// Tme that runner punched control
		wxDateTime m_When;
		bool m_Ignore;
};

#endif // CPUNCH_H