#pragma once

////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////


class WorldFile
{
public:
    WorldFile();
    ~WorldFile();

    enum ECell
    {
        INVALID_CELL = -1, 
        EMPTY_CELL = 0, 
        OCCUPIED_CELL = 1, 
        CELL_MAX
    };

    bool        Load(const LPCWSTR szFilename);

    ECell       operator ()	        ( int row, int col ) const;
	int         GetWidth() const    { return m_cx; }
	int         GetHeight() const   { return m_cy; }

private:
    int m_cx, m_cy;
    ECell* m_pGrid;

	// Default copy constructors won't work because of the ECell* member. 
    // Because I haven't implemented a copy contructor (yet), I'll just 
	// disable them by making them private. 
    WorldFile(const WorldFile&);            // no implementation 
	WorldFile& operator=(const WorldFile&); // no implementation 
};
