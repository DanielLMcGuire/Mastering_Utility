# Daniel's Mastering Utility

Use like so:

```INI
[ MASTER ]

; FORMAT:
; album ID ( string ALBUMNAME, string ARTISTNAME, string COPYRIGHTINFO, string COVERPATH, string PATH, string NEWPATH, string GENRE, string YEAR, string COMMENT )
; { song ID ( string SONGNAME, string ARTISTNAME, int TITLE_NUMBER, string FILE, string NEWFILE, string CODEC, string GENRE, string YEAR, string COMMENT ) }

; NOTE: Currently, ALBUM PATH needs to be set to ./ for a bugfix, which also means THIS file needs to be inside your albums folder!!

album 1 ( "Project Echo Soundtrack", "Daniel McGuire", "(C) Daniel McGuire", "./art.jpg", "./", "../Project Echo Soundtrack/game/", "Soundtrack", "2025", "Project Echo" )
{
	song 1 ( "Endless Echos", "Daniel McGuire", 1, "Endless Echos.wav", "projectecho_song1.mp3" , "libmp3lame", "Soundtrack", "2025", "Project Echo" )
	song 2 ( "Loading Music", "Daniel McGuire", 2, "LoadingMusic.wav", "projectecho_song2.mp3" , "libmp3lame", "Soundtrack", "2025", "Project Echo" )
	song 3 ( "The Echo Starts", "Daniel McGuire", 3, "The Echo Starts.wav", "projectecho_song3.mp3" , "libmp3lame", "Soundtrack", "2025", "Project Echo" )
	song 4 ( "Something New", "Daniel McGuire", 4, "Something New.wav", "projectecho_song4.mp3" , "libmp3lame", "Soundtrack", "2025", "Project Echo" )
	song 5 ( "A New Dimension", "Daniel McGuire", 5, "A New Dimension.wav", "projectecho_song5.mp3" , "libmp3lame", "Soundtrack", "2025", "Project Echo" )
	song 6 ( "Urgency", "Daniel McGuire", 6, "Urgency.wav", "projectecho_song6.mp3" , "libmp3lame", "Soundtrack", "2025", "Project Echo" )
}
; Adding more albums is not currently recommended.
```