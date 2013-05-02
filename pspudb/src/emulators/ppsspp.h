#ifndef PPSSPP_H
#define PPSSPP_H


class ppsspp
{
public:
	enum Mode {
		UMD = 0x10,
		MS0 = 0x11,
		file= 0x12
	};

	static void runPPSSPP(ppsspp::Mode mode,QString path,QWidget *parent);
};

#endif
