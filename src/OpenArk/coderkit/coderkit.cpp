/****************************************************************************
**
** Copyright (C) 2019 BlackINT3
** Contact: https://github.com/BlackINT3/OpenArk
**
** GNU Lesser General Public License Usage (LGPL)
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
****************************************************************************/
#include "coderkit.h"
#include "../common/common.h"
#include "../openark/openark.h"
#include <locale>
#include <codecvt>
#if (!_DLL) && (_MSC_VER >= 1900 /* VS 2015*/) && (_MSC_VER <= 1911 /* VS 2017 */)
std::locale::id std::codecvt<char16_t, char, _Mbstatet>::id;
#endif

typedef std::wstring_convert<std::codecvt_utf8<int16_t>, int16_t> U16Convert;
typedef U16Convert::wide_string U16;
typedef std::wstring_convert<std::codecvt_utf8<int32_t>, int32_t> U32Convert;
typedef U32Convert::wide_string U32;

// Algorithm index
struct {
	int s = 0;
	int base64 = s++;
	int crc32 = s++;
	int md5 = s++;
	int sha1 = s++;
	int rc4 = s++;
	int urlencode = s++;
	int urldecode = s++;
	int urlencodeURL = s++;
	int urldecodeURL = s++;
} IDX;

struct {
	int s = 0;
	int x64 = s++;
	int x86 = s++;
	int x86_16 = s++;
	int arm64 = s++;
	int arm32 = s++;
	int arm16 = s++;
	int mips64 = s++;
	int mips32 = s++;
	int mips16 = s++;
} CPUPLATFORM_IDX;

enum InstrPlatform {
	X64,
	X86,
	X86_16,
	ARM64,
	ARM32,
	ARM16,
	MIPS64,
	MIPS32,
	MIPS16,
};
enum ByteOrder {
	LITTLE_ENDIAN,
	BIG_ENDIAN,
};

CoderKit::CoderKit(QWidget* parent, int tabid) :
	CommonMainTabObject::CommonMainTabObject((OpenArk*)parent)
{
	ui.setupUi(this);
	connect(OpenArkLanguage::Instance(), &OpenArkLanguage::languageChaned, this, [this]() {ui.retranslateUi(this); });

	radio_group_type_.addButton(ui.nullRadio_2, 0);
	radio_group_type_.addButton(ui.spaceRadio_2, 1);
	radio_group_type_.addButton(ui.slashxRadio_2, 2);
	radio_group_type_.addButton(ui.assembleRadio, 3);

	radio_group_interval_.addButton(ui.byteRadio, 0);
	radio_group_interval_.addButton(ui.twoBytesRadio, 1);
	radio_group_interval_.addButton(ui.fourBytesRadio, 2);

	ui.nullRadio_2->setChecked(true);
	ui.byteRadio->setChecked(true);

	connect(ui.nullRadio_2, SIGNAL(clicked()), this, SLOT(onFormatChanged()));
	connect(ui.spaceRadio_2, SIGNAL(clicked()), this, SLOT(onFormatChanged()));
	connect(ui.slashxRadio_2, SIGNAL(clicked()), this, SLOT(onFormatChanged())); 
	connect(ui.assembleRadio, SIGNAL(clicked()), this, SLOT(onFormatChanged()));

	connect(ui.byteRadio, SIGNAL(clicked()), this, SLOT(onFormatChanged()));
	connect(ui.twoBytesRadio, SIGNAL(clicked()), this, SLOT(onFormatChanged()));
	connect(ui.fourBytesRadio, SIGNAL(clicked()), this, SLOT(onFormatChanged()));
	
	connect(ui.textEdit, SIGNAL(textChanged()), this, SLOT(onCodeTextChanged()));
	connect(ui.defaultEdit, SIGNAL(textChanged(const QString &)), this, SLOT(onCodeTextChanged(const QString &)));
	connect(ui.asciiEdit, SIGNAL(textChanged(const QString &)), this, SLOT(onCodeTextChanged(const QString &)));
	connect(ui.unicodeEdit, SIGNAL(textChanged(const QString &)), this, SLOT(onCodeTextChanged(const QString &)));
	connect(ui.utf7Edit, SIGNAL(textChanged(const QString &)), this, SLOT(onCodeTextChanged(const QString &)));
	connect(ui.utf8Edit, SIGNAL(textChanged(const QString &)), this, SLOT(onCodeTextChanged(const QString &)));
	connect(ui.utf16Edit, SIGNAL(textChanged(const QString &)), this, SLOT(onCodeTextChanged(const QString &)));
	connect(ui.butf16Edit, SIGNAL(textChanged(const QString &)), this, SLOT(onCodeTextChanged(const QString &)));
	connect(ui.utf32Edit, SIGNAL(textChanged(const QString &)), this, SLOT(onCodeTextChanged(const QString &)));
	connect(ui.butf32Edit, SIGNAL(textChanged(const QString &)), this, SLOT(onCodeTextChanged(const QString &)));
	connect(ui.gbkEdit, SIGNAL(textChanged(const QString &)), this, SLOT(onCodeTextChanged(const QString &)));
	connect(ui.big5Edit, SIGNAL(textChanged(const QString &)), this, SLOT(onCodeTextChanged(const QString &)));
	connect(ui.cp866Edit, SIGNAL(textChanged(const QString &)), this, SLOT(onCodeTextChanged(const QString &)));

	connect(ui.doserrEdit, SIGNAL(textChanged(const QString &)), this, SLOT(onWindowsErrorTextChanged(const QString &)));
	connect(ui.ntstatusEdit, SIGNAL(textChanged(const QString &)), this, SLOT(onWindowsErrorTextChanged(const QString &)));
	connect(ui.hresultEdit, SIGNAL(textChanged(const QString &)), this, SLOT(onWindowsErrorTextChanged(const QString &)));
	connect(ui.msgidBtn, SIGNAL(clicked()), this, SLOT(onMessageId()));

	alg_idx_ = 0;
	is_user_ = false;
	is_format_changed_ = false;
	onAlgIndexChanged(alg_idx_);
	// ui.typeBox->insertItem(IDX.base64, "Base64");
	// ui.typeBox->insertItem(IDX.crc32, "CRC32");
	// ui.typeBox->insertItem(IDX.md5, "MD5");
	// ui.typeBox->insertItem(IDX.sha1, "SHA1");
	// ui.typeBox->insertItem(IDX.rc4, "RC4");

	ui.base64Radio->setChecked(true);
	connect(ui.base64Radio, SIGNAL(clicked()), this, SLOT(onAlgPlainChanged()));
	connect(ui.crc32Radio, SIGNAL(clicked()), this, SLOT(onAlgPlainChanged()));
	connect(ui.md5Radio, SIGNAL(clicked()), this, SLOT(onAlgPlainChanged()));
	connect(ui.sha1Radio, SIGNAL(clicked()), this, SLOT(onAlgPlainChanged()));
	connect(ui.urlencodeRadio, SIGNAL(clicked()), this, SLOT(onAlgPlainChanged()));
	connect(ui.urldecodeRadio, SIGNAL(clicked()), this, SLOT(onAlgPlainChanged()));
	// connect(ui.urlencodeURLRadio, SIGNAL(clicked()), this, SLOT(onAlgPlainChanged()));
	// connect(ui.urldecodeURLRadio, SIGNAL(clicked()), this, SLOT(onAlgPlainChanged()));
	//connect(ui.typeBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onAlgIndexChanged(int)));
	connect(ui.plainEdit, SIGNAL(textChanged()), this, SLOT(onAlgPlainChanged()));
	connect(ui.cipherEdit, SIGNAL(textChanged()), this, SLOT(onAlgPlainChanged()));
	//connect(ui.keyEdit, SIGNAL(textChanged()), this, SLOT(onAlgPlainChanged()));

	InitAsmToolsView();

	CommonMainTabObject::Init(ui.tabWidget, tabid);
}
 
CoderKit::~CoderKit()
{
}

void CoderKit::onTabChanged(int index)
{
	CommonMainTabObject::onTabChanged(index);
}

void CoderKit::onCodeTextChanged()
{
	std::wstring data;
	std::string str;
	QObject* sender = QObject::sender();
	if (sender == ui.textEdit || is_format_changed_) {
		data = ui.textEdit->toPlainText().toStdWString();
		is_format_changed_ = false;
	}

	UpdateEditCodeText(data, sender);
}

void CoderKit::onCodeTextChanged(const QString & text)
{
	QLineEdit* sender = qobject_cast<QLineEdit*>(QObject::sender());
	sender->setStyleSheet("background-color:white");
	try {
		std::string str = sender->text().toStdString();
		std::wstring data;
		auto InputFilter = [&](std::string& input) {
			UNONE::StrReplaceA(input, "-");
			UNONE::StrReplaceA(input, " ");
			UNONE::StrReplaceA(input, "0x");
			UNONE::StrReplaceA(input, "h");
			UNONE::StrReplaceA(input, "\\x");
			sender->setText(StrToQ(input));
		};
		is_user_ = ui.defaultEdit->isModified()
			|| ui.asciiEdit->isModified()
			|| ui.unicodeEdit->isModified()
			|| ui.utf7Edit->isModified()
			|| ui.utf8Edit->isModified()
			|| ui.utf16Edit->isModified()
			|| ui.butf16Edit->isModified()
			|| ui.utf32Edit->isModified()
			|| ui.butf32Edit->isModified()
			|| ui.gbkEdit->isModified()
			|| ui.big5Edit->isModified()
			|| ui.cp866Edit->isModified();
		if (is_user_) {
			InputFilter(str);
		} else {
			return;
		}
		str = UNONE::StrHexStrToStreamA(str);
		if (sender == ui.defaultEdit) {
			data = UNONE::StrACPToWide(str);
		}	else if (sender == ui.asciiEdit) {
			data = UNONE::StrCodeToWide(437, str);
		}	else if (sender == ui.unicodeEdit) {
			data = std::wstring((wchar_t*)str.c_str(), str.size() / 2);
		} else if (sender == ui.utf7Edit) {
			data = UNONE::StrCodeToWide(CP_UTF7, str);
		}	else if (sender == ui.utf8Edit) {
			data = UNONE::StrCodeToWide(CP_UTF8, str);
		}	else if (sender == ui.utf16Edit) {
			data = std::wstring((wchar_t*)str.c_str(), str.size() / 2);
		}	else if (sender == ui.butf16Edit) {
			str = UNONE::StrReverseA(str, 2);
			data = std::wstring((wchar_t*)str.c_str(), str.size() / 2);
		}	else if (sender == ui.utf32Edit) {
			U32 utf32((int32_t*)str.c_str(), str.size() / 4);
			data = UNONE::StrUTF8ToWide(U32Convert().to_bytes(utf32));
		} else if (sender == ui.butf32Edit) {
			str = UNONE::StrReverseA(str, 4);
			U32 utf32((int32_t*)str.c_str(), str.size() / 4);
			data = UNONE::StrUTF8ToWide(U32Convert().to_bytes(utf32));
		}	else if (sender == ui.gbkEdit) {
			data = UNONE::StrCodeToWide(936, str);
		}	else if (sender == ui.big5Edit) {
			data = UNONE::StrCodeToWide(950, str);
		}	else if (sender == ui.cp866Edit) {
			data = UNONE::StrCodeToWide(866, str);
		}
		UpdateEditCodeText(data, sender);
	} catch(...) {
		sender->setStyleSheet("background-color:red");
	}
}

void CoderKit::onWindowsErrorTextChanged(const QString & text)
{
	std::string number = text.toStdString();
	QLineEdit* sender = qobject_cast<QLineEdit*>(QObject::sender());
	if (sender == ui.doserrEdit) {
		auto err = VariantInt(number, 10);
		auto msg = UNONE::StrFormatW(L"%d: %s", err, UNONE::OsDosErrorMsgW(err).c_str());
		ui.msgEdit->setText(WStrToQ(msg));
	}	else if (sender == ui.ntstatusEdit) {
		auto err = VariantInt(number, 16);
		auto doserr = UNONE::OsNtToDosError((VariantInt(number, 16)));
		auto msg = UNONE::StrFormatW(L"%X: %s", err, UNONE::OsDosErrorMsgW(doserr).c_str());
		ui.doserrEdit->setText(QString("%1").arg(doserr));
		ui.msgEdit->setText(WStrToQ(msg));
	}	else if (sender == ui.hresultEdit) {
		auto hr = VariantInt(number, 16);
		auto doserr = hr & 0xFFFF;
		ui.doserrEdit->setText(QString("%1").arg(doserr));
		auto msg = UNONE::StrFormatW(L"%X: %s", hr, UNONE::OsDosErrorMsgW(doserr).c_str());
		ui.msgEdit->setText(WStrToQ(msg));
	}
}

void CoderKit::onMessageId()
{
	parent_->onExecCmd(L".msg");
	MsgBoxInfo(tr("Open console to view result"));
}

void CoderKit::onAlgIndexChanged(int index)
{
	alg_idx_ = index;

	auto e_key = ui.keyEdit;
	auto e_plain = ui.plainEdit;
	auto l_plain = ui.keyLabel;
	auto e_cipher = ui.cipherEdit;

	e_key->hide();
	l_plain->hide();
	if (index == IDX.rc4) {
		e_key->show();
		l_plain->show();
		UpdateAlgorithmText(false);
		return;
	}

	UpdateAlgorithmText(true);
	return;
}

void CoderKit::onAlgPlainChanged()
{
	auto sender = qobject_cast<QTextEdit*>(QObject::sender());
	if (sender == ui.plainEdit) {
		UpdateAlgorithmText(true);
	} else if (sender == ui.cipherEdit) {
		UpdateAlgorithmText(false);
	} else if (sender == ui.keyEdit) {
		UpdateAlgorithmText(true);
	}
	auto sender_radio = qobject_cast<QRadioButton*>(QObject::sender()); 
	if (sender_radio == ui.base64Radio) {
		alg_idx_ = 0;
	} else if (sender_radio == ui.crc32Radio) {
		alg_idx_ = 1;
	} else if (sender_radio == ui.md5Radio) {
		alg_idx_ = 2;
	} else if (sender_radio == ui.sha1Radio) {
		alg_idx_ = 3;
	} else if (sender_radio == ui.urlencodeRadio) {
		alg_idx_ = 5;
	} else if (sender_radio == ui.urldecodeRadio) {
		alg_idx_ = 6;
	// } else if (sender_radio == ui.urlencodeURLRadio) {
	// 	alg_idx_ = 7;
	// } else if (sender_radio == ui.urldecodeURLRadio) {
	// 	alg_idx_ = 8;
	} else {
		return;
	}
	UpdateAlgorithmText(true);
}

void CoderKit::onFormatChanged()
{
	is_format_changed_ = true;
	onCodeTextChanged();
}

void CoderKit::InitAsmToolsView()
{
	ui.splitter->setStretchFactor(0, 1);
	ui.splitter->setStretchFactor(1, 2);
	ui.nullRadio->setChecked(true);
	connect(ui.asmBtn, &QPushButton::clicked, this, [&]() {
		if (!UNONE::OsIs64()) { MsgBoxError("The feature not support 32bits os."); return; }
		ByteOrder byteorder = LITTLE_ENDIAN;
		auto byteorder_idx = ui.byteorderBox->currentIndex();
		if (byteorder_idx == 0) byteorder = LITTLE_ENDIAN;
		else if (byteorder_idx == 1) byteorder = BIG_ENDIAN;
		InstrPlatform cpu = X64;
		auto idx = ui.platformBox->currentIndex();
		if (idx == CPUPLATFORM_IDX.x64) cpu = X64;
		else if (idx == CPUPLATFORM_IDX.x86) cpu = X86;
		else if (idx == CPUPLATFORM_IDX.x86_16) cpu = X86_16;
		else if (idx == CPUPLATFORM_IDX.arm64) cpu = ARM64;
		else if (idx == CPUPLATFORM_IDX.arm32) cpu = ARM32;
		else if (idx == CPUPLATFORM_IDX.arm16) cpu = ARM16;
		else if (idx == CPUPLATFORM_IDX.mips64) cpu = MIPS64;
		else if (idx == CPUPLATFORM_IDX.mips32) cpu = MIPS32;
		else if (idx == CPUPLATFORM_IDX.mips16) cpu = MIPS16;
		auto &&in = ui.asmEdit->toPlainText().toStdString();

		std::string formats;
		if (ui.nullRadio->isChecked()) formats = "";
		else if (ui.spaceRadio->isChecked()) formats = " ";
		else if (ui.slashxRadio->isChecked()) formats = "\\x";

		auto &&out = Rasm2Asm(in, cpu, byteorder, formats);
		ui.disasmEdit->setText(out);
	});

	connect(ui.disasmBtn, &QPushButton::clicked, this, [&]() {
		if (!UNONE::OsIs64()) { MsgBoxError("The feature not support 32bits os."); return; }
		ByteOrder byteorder = LITTLE_ENDIAN;
		auto byteorder_idx = ui.byteorderBox->currentIndex();
		if (byteorder_idx == 0) byteorder = LITTLE_ENDIAN;
		else if (byteorder_idx == 1) byteorder = BIG_ENDIAN;
		InstrPlatform cpu = X64;
		auto idx = ui.platformBox->currentIndex();
		if (idx == CPUPLATFORM_IDX.x64) cpu = X64;
		else if (idx == CPUPLATFORM_IDX.x86) cpu = X86;
		else if (idx == CPUPLATFORM_IDX.x86_16) cpu = X86_16;
		else if (idx == CPUPLATFORM_IDX.arm64) cpu = ARM64;
		else if (idx == CPUPLATFORM_IDX.arm32) cpu = ARM32;
		else if (idx == CPUPLATFORM_IDX.arm16) cpu = ARM16;
		else if (idx == CPUPLATFORM_IDX.mips64) cpu = MIPS64;
		else if (idx == CPUPLATFORM_IDX.mips32) cpu = MIPS32;
		else if (idx == CPUPLATFORM_IDX.mips16) cpu = MIPS16;
		auto &&in = ui.asmEdit->toPlainText().toStdString();
		const char *pfx = "file:///";
		auto pos = in.find(pfx);
		if (pos == 0) {
			auto file = UNONE::StrToW(in.substr(pos + strlen(pfx)));
			UNONE::FsReadFileDataW(file, in);
			in = UNONE::StrStreamToHexStrA(in);
		} else {
			UNONE::StrReplaceA(in, " ");
			UNONE::StrReplaceA(in, "\\x");
		}
		if (in.size() >= 10 * KB) {
			auto msbox = QMessageBox::warning(this, tr("Warning"),
				tr("Your input data so much(suggest less 10 KB), it'll be very slowly, continue?"),
				QMessageBox::Yes | QMessageBox::No);
			if (msbox == QMessageBox::No) return;
		}

		auto &&out = Rasm2Disasm(in, cpu, byteorder);
		ui.disasmEdit->setText(out);
	});
}

void CoderKit::UpdateAlgorithmText(bool crypt)
{
	auto e_key = ui.keyEdit;
	std::string key = e_key->toPlainText().toStdString();

	auto e_plain = ui.plainEdit;
	std::string plain = e_plain->toPlainText().toStdString();

	auto e_cipher = ui.cipherEdit;
	std::string cipher;

	if (alg_idx_ == IDX.base64) {
		if (crypt) {
			cipher = Cryptor::Base64Encode(plain);
		} else {
			cipher = e_cipher->toPlainText().toStdString();
			plain = Cryptor::Base64Decode(cipher);
			e_plain->blockSignals(true);
			e_plain->setText(StrToQ(plain));
			e_plain->blockSignals(false);
			return;
		}
	}	else if (alg_idx_ == IDX.crc32) {
		auto val = Cryptor::GetCRC32ByData(plain);
		cipher = UNONE::StrFormatA("%x", val);
	} else if (alg_idx_ == IDX.md5) {
		cipher = Cryptor::GetMD5ByData(plain);
		cipher = UNONE::StrStreamToHexStrA(cipher);
	}	else if (alg_idx_ == IDX.sha1) {
		cipher = Cryptor::GetSHA1ByData(plain);
		cipher = UNONE::StrStreamToHexStrA(cipher);
	}	else if (alg_idx_ == IDX.rc4) {
		cipher = Cryptor::GetSHA1ByData(plain);
	} else if (alg_idx_ == IDX.urlencode) {
		std::vector<char> pass;
		std::vector<std::string> headers = { "http", "https", "ftp" };
		for (auto h : headers) {
			if (UNONE::StrIndexIA(plain, h) == 0) {
				pass = { ':', '&', '/', '?', '=' };
				break;
			}
		}
		cipher = UrlEncode(plain, pass);
	} else if (alg_idx_ == IDX.urldecode) {
		cipher = UrlDecode(plain);
	} else if (alg_idx_ == IDX.urlencodeURL) {
		cipher = UrlEncodeURL(plain);
	}	else if (alg_idx_ == IDX.urldecodeURL) {
		cipher = UrlDecode(plain);
	}

	if (!crypt) return;
	e_cipher->blockSignals(true);
	e_cipher->setText(StrToQ(cipher));
	e_cipher->blockSignals(false);
}

void CoderKit::UpdateEditCodeText(const std::wstring& data, QObject* ignored_obj)
{
	//prevent multi call simultaneously
	std::unique_lock<std::mutex> guard(upt_mutex_, std::try_to_lock);
	if (!guard.owns_lock()) return;

	auto SetText = [&](QObject* obj, std::string data) {
		if (obj == ignored_obj) return;
		const char* class_name = obj->metaObject()->className();
		if (class_name == QStringLiteral("QTextEdit")) {
			qobject_cast<QTextEdit*>(obj)->setText(StrToQ(data));
		}	else if (class_name == QStringLiteral("QLineEdit")) {
			data = UNONE::StrTrimA(data);
			qobject_cast<QLineEdit*>(obj)->setText(StrToQ(data));
		}
	};

	is_user_ = false;

	int interval = 2;
	int id_interval = radio_group_interval_.checkedId();
	if (id_interval == 0) interval = 2;
	else if (id_interval == 1) interval = 4;
	else if (id_interval == 2) interval = 8;

	std::string format = "";
	int id_format = radio_group_type_.checkedId();
	if (id_format == 0) format = "";
	else if (id_format == 1) format = " ";
	else if (id_format == 2) format = "\\x";
	else if (id_format == 3) format = "h, ", interval = 2;

	std::string text;
	text = UNONE::StrWideToUTF8(data);
	SetText(ui.textEdit, text);
	
	text = UNONE::StrStreamToHexStrA(UNONE::StrWideToACP(data));
	SolveCodeTextFormat(text, format, interval, id_format);
	SetText(ui.defaultEdit, text);

	text = UNONE::StrStreamToHexStrA(UNONE::StrACPToCode(437, UNONE::StrToA(data)));
	SolveCodeTextFormat(text, format, interval, id_format);
	SetText(ui.asciiEdit, text);

	text = UNONE::StrStreamToHexStrA(std::string((char*)data.c_str(), data.size() * 2));
	SolveCodeTextFormat(text, format, interval, id_format);
	SetText(ui.unicodeEdit, text);

	text = UNONE::StrStreamToHexStrA(UNONE::StrWideToCode(CP_UTF7, data));
	SolveCodeTextFormat(text, format, interval, id_format);
	SetText(ui.utf7Edit, text);

	text = UNONE::StrStreamToHexStrA(UNONE::StrWideToCode(CP_UTF8, data));
	SolveCodeTextFormat(text, format, interval, id_format);
	SetText(ui.utf8Edit, text);

	text = UNONE::StrStreamToHexStrA(std::string((char*)data.c_str(), data.size() * 2));
	SolveCodeTextFormat(text, format, interval, id_format);
	SetText(ui.utf16Edit, text);

	auto stream = std::string((char*)data.c_str(), data.size() * 2);
	stream = UNONE::StrReverseA(stream, 2);
	text = UNONE::StrStreamToHexStrA(stream);
	SolveCodeTextFormat(text, format, interval, id_format);
	SetText(ui.butf16Edit, text);

	U32Convert cvt32;
	auto utf32 = cvt32.from_bytes(UNONE::StrWideToCode(CP_UTF8, data));
	stream = std::string((char*)utf32.c_str(), utf32.size() * 4);
	text = UNONE::StrStreamToHexStrA(stream);
	SolveCodeTextFormat(text, format, interval, id_format);
	SetText(ui.utf32Edit, text);

	stream = UNONE::StrReverseA(stream, 4);
	text = UNONE::StrStreamToHexStrA(stream);
	SolveCodeTextFormat(text, format, interval, id_format);
	SetText(ui.butf32Edit, text);

	text = UNONE::StrStreamToHexStrA(UNONE::StrWideToCode(936, data));
	SolveCodeTextFormat(text, format, interval, id_format);
	SetText(ui.gbkEdit, text);

	text = UNONE::StrStreamToHexStrA(UNONE::StrWideToCode(950, data));
	SolveCodeTextFormat(text, format, interval, id_format);
	SetText(ui.big5Edit, text);

	text = UNONE::StrStreamToHexStrA(UNONE::StrWideToCode(866, data));
	SolveCodeTextFormat(text, format, interval, id_format);
	SetText(ui.cp866Edit, text);
}

QString CoderKit::Rasm2Asm(std::string data, int cpu, int byteorder, const std::string &format)
{
	auto &&rasm2 = AppConfigDir() + L"\\nasm\\rasm2.exe";
	if (!UNONE::FsIsExistedW(rasm2)) {
		ExtractResource(":/OpenArk/nasm/rasm2.exe", WStrToQ(rasm2));
	}
	auto &&tmp_out = UNONE::OsEnvironmentW(L"%Temp%\\temp-nasm-code.bin");
	auto &&tmp_in = UNONE::OsEnvironmentW(L"%Temp%\\temp-nasm-code.asm");
	UNONE::FsWriteFileDataW(tmp_in, data);
	int bits;
	std::wstring plat;
	switch (cpu) {
	case X64: plat = L"x86";  bits = 64; break;
	case X86: plat = L"x86";  bits = 32; break;
	case X86_16: plat = L"x86";  bits = 16; break;
	case ARM64: plat = L"arm";  bits = 64; break;
	case ARM32: plat = L"arm";  bits = 32; break;
	case ARM16: plat = L"arm";  bits = 16; break;
	case MIPS64: plat = L"mips";  bits = 64; break;
	case MIPS32: plat = L"mips";  bits = 32; break;
	case MIPS16: plat = L"mips";  bits = 16; break;
	}
	auto &&cmdline = rasm2;
	if (byteorder == BIG_ENDIAN) cmdline += L" -e";
	cmdline = UNONE::StrFormatW(L"%s -a %s -b%d -O \"%s\" -f \"%s\"",
		rasm2.c_str(), plat.c_str(), bits,
		tmp_out.c_str(), tmp_in.c_str());

	std::wstring out;
	DWORD exitcode;
	QString err_prefix = tr("Compile Error:\n--------------------------------------------------------------\n");
	auto ret = ReadStdout(cmdline, out, exitcode);
	if (!ret) return err_prefix + tr("run compiler error");
	if (exitcode != 0) return err_prefix + WStrToQ(out);
	std::string bin;
	UNONE::FsReadFileDataW(tmp_out, bin);
	bin = format + UNONE::StrInsertA(bin, 2, format);
	return StrToQ(bin);
}

QString CoderKit::Rasm2Disasm(std::string data, int cpu, int byteorder)
{
	auto &&rasm2 = AppConfigDir() + L"\\nasm\\rasm2.exe";
	if (!UNONE::FsIsExistedW(rasm2)) {
		ExtractResource(":/OpenArk/nasm/rasm2.exe", WStrToQ(rasm2));
	}
	auto &&tmp_in = UNONE::OsEnvironmentW(L"%Temp%\\temp-ndisasm-code.bin");
	UNONE::FsWriteFileDataW(tmp_in, data);
	int bits;
	std::wstring plat;
	switch (cpu) {
	case X64: plat = L"x86";  bits = 64; break;
	case X86: plat = L"x86";  bits = 32; break;
	case X86_16: plat = L"x86";  bits = 16; break;
	case ARM64: plat = L"arm";  bits = 64; break;
	case ARM32: plat = L"arm";  bits = 32; break;
	case ARM16: plat = L"arm";  bits = 16; break;
	case MIPS64: plat = L"mips";  bits = 64; break;
	case MIPS32: plat = L"mips";  bits = 32; break;
	case MIPS16: plat = L"mips";  bits = 16; break;
	}
	auto &&cmdline = rasm2;
	if (byteorder == BIG_ENDIAN) cmdline += L" -e";
	cmdline = UNONE::StrFormatW(L"%s -a %s -b%d -D -f \"%s\"",
		cmdline.c_str(), plat.c_str(), bits,
		tmp_in.c_str());
	std::wstring out;
	DWORD exitcode;
	auto ret = ReadStdout(cmdline, out, exitcode);
	if (!ret) return tr("run disassember error");
	UNONE::StrLowerW(out);
	//UNONE::StrReplaceW(out, L"                 ", L"    ");
	return WStrToQ(out);
}

QString CoderKit::NasmAsm(std::string data, int bits, const std::string &format)
{
	if (bits == 64) data.insert(0, "[bits 64]\n");
	else if (bits == 32) data.insert(0, "[bits 32]\n");
	else if (bits == 16) data.insert(0, "[bits 16]\n");

	auto &&nasm = AppConfigDir() + L"\\nasm\\nasm.exe";
	if (!UNONE::FsIsExistedW(nasm)) {
		ExtractResource(":/OpenArk/nasm/nasm.exe", WStrToQ(nasm));
	}
	auto &&tmp_out = UNONE::OsEnvironmentW(L"%Temp%\\temp-nasm-code.bin");
	auto &&tmp_in = UNONE::OsEnvironmentW(L"%Temp%\\temp-nasm-code.asm");
	UNONE::FsWriteFileDataW(tmp_in, data);
	auto &&cmdline = UNONE::StrFormatW(L"%s -f bin -o \"%s\" \"%s\"", nasm.c_str(), tmp_out.c_str(), tmp_in.c_str());
	std::wstring out;
	DWORD exitcode;
	QString err_prefix = tr("Compile Error:\n--------------------------------------------------------------\n");
	auto ret = ReadStdout(cmdline, out, exitcode);
	if (!ret) return err_prefix + tr("start nasm error");
	if (exitcode != 0) return err_prefix + WStrToQ(out);
	std::string bin;
	UNONE::FsReadFileDataW(tmp_out, bin);

	bin = UNONE::StrStreamToHexStrA(bin);
	bin = format + UNONE::StrInsertA(bin, 2, format);
	return StrToQ(bin);
}

QString CoderKit::NasmDisasm(const std::string &data, int bits)
{
	auto &&ndisasm = AppConfigDir() + L"\\nasm\\ndisasm.exe";
	if (!UNONE::FsIsExistedW(ndisasm)) {
		ExtractResource(":/OpenArk/nasm/ndisasm.exe", WStrToQ(ndisasm));
	}
	auto &&tmp_in = UNONE::OsEnvironmentW(L"%Temp%\\temp-ndisasm-code.bin");
	UNONE::FsWriteFileDataW(tmp_in, data);
	auto &&cmdline = UNONE::StrFormatW(L"%s -b %d \"%s\"", ndisasm.c_str(), bits, tmp_in.c_str());
	std::wstring out;
	DWORD exitcode;
	auto ret = ReadStdout(cmdline, out, exitcode);
	if (!ret) return tr("start ndisasm error");
	UNONE::StrLowerW(out);
	return WStrToQ(out);
}

void CoderKit::SolveCodeTextFormat(std::string &text, std::string &format, int interval, int id)
{
	
	if (id == 3) {
		// assemble
		text = UNONE::StrInsertA(text, interval, format);
		text = text + "h";
	} else {
		text = format + UNONE::StrInsertA(text, interval, format);
	}
}

#ifndef HEX_TO_UPPER_CHAR
#define HEX_TO_UPPER_CHAR(x)	((unsigned char)(x) > 9 ? (unsigned char)(x) -10 + 'A': (unsigned char)(x) + '0')
#endif
//'1' => 1 / 'A' => A
#ifndef UPPER_CHAR_TO_HEX
#define UPPER_CHAR_TO_HEX(x)	(isdigit((unsigned char)(x)) ? (unsigned char)(x)-'0' : (unsigned char)(toupper(x))-'A'+10)
#endif

static const char* kUrlReservedCharset = "!*'();:@&=+$,/?#[]";
static const char* kUrlNonReservedCharset = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_.~";

std::string CoderKit::UrlEncode(const std::string &buf, std::vector<char> pass)
{
	std::string str;
	try {
		std::string CharSet = kUrlNonReservedCharset;
		for (size_t i = 0; i < buf.size(); i++) {
			unsigned char temp[4] = {0};
			unsigned char ch = static_cast<unsigned char>(buf[i]);
			bool found = false;
			for (auto p : pass) {
				if (p == ch) {
					found = true;
					break;
				}
			}
			if (found || CharSet.find(ch) != std::string::npos) {
				temp[0] = ch;
			} else {
				temp[0] = '%';
				temp[1] = HEX_TO_UPPER_CHAR(ch >> 4);
				temp[2] = HEX_TO_UPPER_CHAR(ch & 0x0F);
			}
			str += (char*)temp;
		}
	} catch (std::exception& e) {
		str.clear();
	} catch (...) {
		str.clear();
	}
	return std::move(str);
}

std::string CoderKit::UrlDecode(const std::string &buf) 
{
	std::string str;	
	try {
		std::string bits;
		bits.assign(buf.size(), 0);
		for (size_t i = 0; i < buf.size(); i++) {
			if (buf[i]=='%') {
				bits[i] = 1;
				bits[i+1] = 1;
				bits[i+2] = 1;
				i += 2;
			}
		}
		auto& decode = [](std::string& s)->std::string {
			std::string out;
			for (size_t i = 0; i < s.size(); i+=3) {
				unsigned char ch = 0;
				if (s[i] != '%')
					continue;
				ch = UPPER_CHAR_TO_HEX(s[i+1]) << 4;
				ch |= (UPPER_CHAR_TO_HEX(s[i+2]) & 0x0F);
				out.push_back(ch);
			}
			//out = StrUTF8ToGBK(out);
			return out;
		};
		size_t last = 0;
		size_t i = 0;
		for (i=0; i<bits.size(); i++) {
			if (bits[i] == 0) {
				if (last < i) {
					str += decode(buf.substr(last, i-last));
				}
				str += buf[i];
				last = i + 1;
			}
		}
		//处理边界情况
		if (bits.size() && bits.back() == 1) {
			if (last < i) {
				str += decode(buf.substr(last, i-last));
			}
		}

	} catch (std::exception& e) {
		str.clear();
	} catch (...) {
		str.clear();
	}
	return std::move(str);
}


bool IsValidUrlChar(char ch, bool unsafe_only) {
  if (static_cast<unsigned>(ch) < 0 || static_cast<unsigned>(ch) > 127) {
	  return false;
  }
  if (unsafe_only) {
    return !(ch <= ' ' || strchr("\\\"^&`<>[]{}", ch));
  } else {
    return isalnum(ch) || strchr("-_.!~*'()", ch);
  }
}

static const char* kSpecialCharset = "/:?";
static const char* kNewUrlNonReservedCharset = "!*()ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_.~";

std::string CoderKit::UrlEncodeURL(const std::string &buf) 
{
	std::string source = buf;
	std::string str;
	try {
		// for (size_t i = 0; i < buf.size(); i++) {
		// 	unsigned char temp[4] = {0};
		// 	unsigned char ch = static_cast<unsigned char>(buf[i]);
		// 	if (char_set.find(ch) != std::string::npos) {
		// 		temp[0] = ch;
		// 	}
		// 	else {
		// 		if (!has_question_mark && (special_charset.find(ch) != std::string::npos)) {
		// 			temp[0] = ch;
		// 			has_question_mark = (ch == '?');
		// 		} else if (!has_equal_mark && ch == '=') {
		// 			temp[0] = ch;
		// 			has_equal_mark = true;
		// 		} else {
		// 			temp[0] = '%';
		// 			temp[1] = HEX_TO_UPPER_CHAR(ch >> 4);
		// 			temp[2] = HEX_TO_UPPER_CHAR(ch & 0x0F);
		// 		}
		// 	}
		// 	str += (char*)temp;
		// }
	} catch (std::exception& e) {
		str.clear();
	} catch (...) {
		str.clear();
	}
	return std::move(str);
	// size_t size = buf.size() + 5;
	// std::unique_ptr<char[]> source_ptr(new char[size]);
	// std::unique_ptr<char[]> dest_ptr(new char[size<<2]);
	// strcpy(source_ptr.get(), buf.c_str());
	// char *source = source_ptr.get();
	// char *dest = dest_ptr.get();
	// bool encode_space_as_plus = true;
	// bool unsafe_only = false;

	// static const char *digits = "0123456789ABCDEF";
  // //if (max == 0) {
  // //  return 0;
  // //}
  // char *start = dest;
  // while (*source) {
  //   unsigned char ch = static_cast<unsigned char>(*source);
  //   if (*source == ' ' && encode_space_as_plus && !unsafe_only) {
  //     *dest++ = '+';
  //   } else if (IsValidUrlChar(ch, unsafe_only)) {
  //     *dest++ = *source;
  //   } else {
  //     /*if (static_cast<unsigned>(dest - start) + 4 > max) {
  //       break;
  //     }*/
  //     *dest++ = '%';
  //     *dest++ = digits[(ch >> 4) & 0x0F];
  //     *dest++ = digits[       ch & 0x0F];
  //   }
  //   source++;
  // }
  // // ASSERT(static_cast<unsigned int>(dest - start) < max);
  // *dest = 0;
  // return std::move(dest_ptr.get());
}


