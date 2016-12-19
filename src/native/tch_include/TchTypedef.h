namespace Tnelab{
	enum TchAppStartPosition { Manual, CenterScreen };
	typedef struct {
		char* Url;
		TchAppStartPosition StartPosition;
		int X;
		int Y;
		int Width;
		int Height;
	}TchAppStartSettings;
}
