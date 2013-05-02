#ifndef JPCSP_H
#define JPCSP_H


class Jpcsp
{
public:
	enum Mode {
		UMD = 0x10,
		MS0 = 0x11,
		file= 0x12
	};

	static void runJPCSP(Jpcsp::Mode mode,QString path,QWidget *parent);
};

#endif
