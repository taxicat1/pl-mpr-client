import argparse
import struct
import re
import json


def file_read_u32_le(file):
	bytes = file.read(4)
	return struct.unpack("<L", bytes)[0]


def file_read_u16_le(file):
	bytes = file.read(2)
	return struct.unpack("<H", bytes)[0]


def file_write_u32_le(file, val):
	file.write(struct.pack("<L", val))
	

def file_write_u16_le(file, val):
	file.write(struct.pack("<H", val))


char_lookup = {
	0x0001 : "\u3000", 0x0002 : "ぁ", 0x0003 : "あ", 0x0004 : "ぃ", 0x0005 : "い", 0x0006 : "ぅ", 0x0007 : "う", 0x0008 : "ぇ",
	0x0009 : "え", 0x000A : "ぉ", 0x000B : "お", 0x000C : "か", 0x000D : "が", 0x000E : "き", 0x000F : "ぎ", 0x0010 : "く",
	0x0011 : "ぐ", 0x0012 : "け", 0x0013 : "げ", 0x0014 : "こ", 0x0015 : "ご", 0x0016 : "さ", 0x0017 : "ざ", 0x0018 : "し",
	0x0019 : "じ", 0x001A : "す", 0x001B : "ず", 0x001C : "せ", 0x001D : "ぜ", 0x001E : "そ", 0x001F : "ぞ", 0x0020 : "た",
	0x0021 : "だ", 0x0022 : "ち", 0x0023 : "ぢ", 0x0024 : "っ", 0x0025 : "つ", 0x0026 : "づ", 0x0027 : "て", 0x0028 : "で",
	0x0029 : "と", 0x002A : "ど", 0x002B : "な", 0x002C : "に", 0x002D : "ぬ", 0x002E : "ね", 0x002F : "の", 0x0030 : "は",
	0x0031 : "ば", 0x0032 : "ぱ", 0x0033 : "ひ", 0x0034 : "び", 0x0035 : "ぴ", 0x0036 : "ふ", 0x0037 : "ぶ", 0x0038 : "ぷ",
	0x0039 : "へ", 0x003A : "べ", 0x003B : "ぺ", 0x003C : "ほ", 0x003D : "ぼ", 0x003E : "ぽ", 0x003F : "ま", 0x0040 : "み",
	0x0041 : "む", 0x0042 : "め", 0x0043 : "も", 0x0044 : "ゃ", 0x0045 : "や", 0x0046 : "ゅ", 0x0047 : "ゆ", 0x0048 : "ょ",
	0x0049 : "よ", 0x004A : "ら", 0x004B : "り", 0x004C : "る", 0x004D : "れ", 0x004E : "ろ", 0x004F : "わ", 0x0050 : "を",
	0x0051 : "ん", 0x0052 : "ァ", 0x0053 : "ア", 0x0054 : "ィ", 0x0055 : "イ", 0x0056 : "ゥ", 0x0057 : "ウ", 0x0058 : "ェ",
	0x0059 : "エ", 0x005A : "ォ", 0x005B : "オ", 0x005C : "カ", 0x005D : "ガ", 0x005E : "キ", 0x005F : "ギ", 0x0060 : "ク",
	0x0061 : "グ", 0x0062 : "ケ", 0x0063 : "ゲ", 0x0064 : "コ", 0x0065 : "ゴ", 0x0066 : "サ", 0x0067 : "ザ", 0x0068 : "シ",
	0x0069 : "ジ", 0x006A : "ス", 0x006B : "ズ", 0x006C : "セ", 0x006D : "ゼ", 0x006E : "ソ", 0x006F : "ゾ", 0x0070 : "タ",
	0x0071 : "ダ", 0x0072 : "チ", 0x0073 : "ヂ", 0x0074 : "ッ", 0x0075 : "ツ", 0x0076 : "ヅ", 0x0077 : "テ", 0x0078 : "デ",
	0x0079 : "ト", 0x007A : "ド", 0x007B : "ナ", 0x007C : "ニ", 0x007D : "ヌ", 0x007E : "ネ", 0x007F : "ノ", 0x0080 : "ハ",
	0x0081 : "バ", 0x0082 : "パ", 0x0083 : "ヒ", 0x0084 : "ビ", 0x0085 : "ピ", 0x0086 : "フ", 0x0087 : "ブ", 0x0088 : "プ",
	0x0089 : "ヘ", 0x008A : "ベ", 0x008B : "ペ", 0x008C : "ホ", 0x008D : "ボ", 0x008E : "ポ", 0x008F : "マ", 0x0090 : "ミ",
	0x0091 : "ム", 0x0092 : "メ", 0x0093 : "モ", 0x0094 : "ャ", 0x0095 : "ヤ", 0x0096 : "ュ", 0x0097 : "ユ", 0x0098 : "ョ",
	0x0099 : "ヨ", 0x009A : "ラ", 0x009B : "リ", 0x009C : "ル", 0x009D : "レ", 0x009E : "ロ", 0x009F : "ワ", 0x00A0 : "ヲ",
	0x00A1 : "ン", 0x00A2 : "０", 0x00A3 : "１", 0x00A4 : "２", 0x00A5 : "３", 0x00A6 : "４", 0x00A7 : "５", 0x00A8 : "６",
	0x00A9 : "７", 0x00AA : "８", 0x00AB : "９", 0x00AC : "Ａ", 0x00AD : "Ｂ", 0x00AE : "Ｃ", 0x00AF : "Ｄ", 0x00B0 : "Ｅ",
	0x00B1 : "Ｆ", 0x00B2 : "Ｇ", 0x00B3 : "Ｈ", 0x00B4 : "Ｉ", 0x00B5 : "Ｊ", 0x00B6 : "Ｋ", 0x00B7 : "Ｌ", 0x00B8 : "Ｍ",
	0x00B9 : "Ｎ", 0x00BA : "Ｏ", 0x00BB : "Ｐ", 0x00BC : "Ｑ", 0x00BD : "Ｒ", 0x00BE : "Ｓ", 0x00BF : "Ｔ", 0x00C0 : "Ｕ",
	0x00C1 : "Ｖ", 0x00C2 : "Ｗ", 0x00C3 : "Ｘ", 0x00C4 : "Ｙ", 0x00C5 : "Ｚ", 0x00C6 : "ａ", 0x00C7 : "ｂ", 0x00C8 : "ｃ",
	0x00C9 : "ｄ", 0x00CA : "ｅ", 0x00CB : "ｆ", 0x00CC : "ｇ", 0x00CD : "ｈ", 0x00CE : "ｉ", 0x00CF : "ｊ", 0x00D0 : "ｋ",
	0x00D1 : "ｌ", 0x00D2 : "ｍ", 0x00D3 : "ｎ", 0x00D4 : "ｏ", 0x00D5 : "ｐ", 0x00D6 : "ｑ", 0x00D7 : "ｒ", 0x00D8 : "ｓ",
	0x00D9 : "ｔ", 0x00DA : "ｕ", 0x00DB : "ｖ", 0x00DC : "ｗ", 0x00DD : "ｘ", 0x00DE : "ｙ", 0x00DF : "ｚ", 0x00E0 : "",
	0x00E1 : "！", 0x00E2 : "？", 0x00E3 : "、", 0x00E4 : "。", 0x00E5 : "…", 0x00E6 : "・", 0x00E7 : "／", 0x00E8 : "「",
	0x00E9 : "」", 0x00EA : "『", 0x00EB : "』", 0x00EC : "（", 0x00ED : "）", 0x00EE : "♂", 0x00EF : "♀", 0x00F0 : "＋",
	0x00F1 : "ー", 0x00F2 : "×", 0x00F3 : "÷", 0x00F4 : "＝", 0x00F5 : "～", 0x00F6 : "：", 0x00F7 : "；", 0x00F8 : "．",
	0x00F9 : "，", 0x00FA : "♠", 0x00FB : "♣", 0x00FC : "♥", 0x00FD : "♦", 0x00FE : "★", 0x00FF : "◎", 0x0100 : "○",
	0x0101 : "□", 0x0102 : "△", 0x0103 : "◇", 0x0104 : "＠", 0x0105 : "♪", 0x0106 : "％", 0x0107 : "☀", 0x0108 : "☁",
	0x0109 : "☂", 0x010A : "☃", 0x010B : "😑︎", 0x010C : "☺", 0x010D : "☹", 0x010E : "😠︎", 0x010F : "⤴︎", 0x0110 : "⤵︎",
	0x0111 : "💤︎", 0x0112 : "円", 0x011B : "←", 0x011C : "↑", 0x011D : "↓", 0x011E : "→", 0x011F : "►", 0x0120 : "＆",
	0x0121 : "0", 0x0122 : "1", 0x0123 : "2", 0x0124 : "3", 0x0125 : "4", 0x0126 : "5", 0x0127 : "6", 0x0128 : "7",
	0x0129 : "8", 0x012A : "9", 0x012B : "A", 0x012C : "B", 0x012D : "C", 0x012E : "D", 0x012F : "E", 0x0130 : "F",
	0x0131 : "G", 0x0132 : "H", 0x0133 : "I", 0x0134 : "J", 0x0135 : "K", 0x0136 : "L", 0x0137 : "M", 0x0138 : "N",
	0x0139 : "O", 0x013A : "P", 0x013B : "Q", 0x013C : "R", 0x013D : "S", 0x013E : "T", 0x013F : "U", 0x0140 : "V",
	0x0141 : "W", 0x0142 : "X", 0x0143 : "Y", 0x0144 : "Z", 0x0145 : "a", 0x0146 : "b", 0x0147 : "c", 0x0148 : "d",
	0x0149 : "e", 0x014A : "f", 0x014B : "g", 0x014C : "h", 0x014D : "i", 0x014E : "j", 0x014F : "k", 0x0150 : "l",
	0x0151 : "m", 0x0152 : "n", 0x0153 : "o", 0x0154 : "p", 0x0155 : "q", 0x0156 : "r", 0x0157 : "s", 0x0158 : "t",
	0x0159 : "u", 0x015A : "v", 0x015B : "w", 0x015C : "x", 0x015D : "y", 0x015E : "z", 0x015F : "À", 0x0160 : "Á",
	0x0161 : "Â", 0x0162 : "Ã", 0x0163 : "Ä", 0x0164 : "Å", 0x0165 : "Æ", 0x0166 : "Ç", 0x0167 : "È", 0x0168 : "É",
	0x0169 : "Ê", 0x016A : "Ë", 0x016B : "Ì", 0x016C : "Í", 0x016D : "Î", 0x016E : "Ï", 0x016F : "Ð", 0x0170 : "Ñ",
	0x0171 : "Ò", 0x0172 : "Ó", 0x0173 : "Ô", 0x0174 : "Õ", 0x0175 : "Ö", 0x0176 : "×", 0x0177 : "Ø", 0x0178 : "Ù",
	0x0179 : "Ú", 0x017A : "Û", 0x017B : "Ü", 0x017C : "Ý", 0x017D : "Þ", 0x017E : "ß", 0x017F : "à", 0x0180 : "á",
	0x0181 : "â", 0x0182 : "ã", 0x0183 : "ä", 0x0184 : "å", 0x0185 : "æ", 0x0186 : "ç", 0x0187 : "è", 0x0188 : "é",
	0x0189 : "ê", 0x018A : "ë", 0x018B : "ì", 0x018C : "í", 0x018D : "î", 0x018E : "ï", 0x018F : "ð", 0x0190 : "ñ",
	0x0191 : "ò", 0x0192 : "ó", 0x0193 : "ô", 0x0194 : "õ", 0x0195 : "ö", 0x0196 : "÷", 0x0197 : "ø", 0x0198 : "ù",
	0x0199 : "ú", 0x019A : "û", 0x019B : "ü", 0x019C : "ý", 0x019D : "þ", 0x019E : "ÿ", 0x019F : "Œ", 0x01A0 : "œ",
	0x01A1 : "Ş", 0x01A2 : "ş", 0x01A3 : "ª", 0x01A4 : "º", 0x01A8 : "$", 0x01A9 : "¡", 0x01AA : "¿", 0x01AB : "!",
	0x01AC : "?", 0x01AD : ",", 0x01AE : ".", 0x01AF : "…", 0x01B0 : "･", 0x01B1 : "/", 0x01B2 : "‘", 0x01B3 : "'",
	0x01B4 : "“", 0x01B5 : "”", 0x01B6 : "„", 0x01B7 : "«", 0x01B8 : "»", 0x01B9 : "(", 0x01BA : ")", 0x01BB : "♂",
	0x01BC : "♀", 0x01BD : "+", 0x01BE : "-", 0x01BF : "*", 0x01C0 : "#", 0x01C1 : "=", 0x01C2 : "&", 0x01C3 : "~",
	0x01C4 : ":", 0x01C5 : ";", 0x01C6 : "♠", 0x01C7 : "♣", 0x01C8 : "♥", 0x01C9 : "♦", 0x01CA : "★", 0x01CB : "◎",
	0x01CC : "○", 0x01CD : "□", 0x01CE : "△", 0x01CF : "◇", 0x01DE : " ", 0xE000 : "\n"
}

char_reverse_lookup = { v:k for k,v in char_lookup.items() }


def pokestr_decode(poke_str):
	output = ""
	next_is_command = False
	next_is_argument = False
	for char in poke_str:
		if char == 0xFFFF:
			break
		
		if not next_is_command and not next_is_argument and char in char_lookup:
			output += char_lookup[char]
		else:
			output += f"[{char:04X}]"
		
		# Argument follows command
		# [FFFE][command][argument]
		next_is_argument = next_is_command
		next_is_command = (char == 0xFFFE)
	
	return output


def pokestr_encode(str):
	output = []
	i = 0;
	while i != len(str):
		if str[i] in char_reverse_lookup:
			output.append(char_reverse_lookup[str[i]])
			i += 1
		else:
			code = str[i:i+6]
			if re.fullmatch(r"\[[0-9A-Fa-f]{4}\]", code):
				output.append(int(code[1:5], 16))
				i += 6
			else:
				raise ValueError("Could not encode character " + str[i])
	
	output.append(0xFFFF)
	return output


def file_read_pokestring(file):
	str_chars = []
	while True:
		wchar = file_read_u16_le(file)
		str_chars.append(wchar)
		if wchar == 0xFFFF:
			break
	
	return pokestr_decode(str_chars)


def file_write_pokestring(file, str):
	for char in str:
		file_write_u16_le(file, char)


def pack_json(json_file, output):
	if output is None:
		raise ValueError("Must provide file for binary output")
	
	json_data = json.load(json_file)
	
	# Write magic
	file_write_u32_le(output, json_data["magic"])
	
	header_space = 4 + (len(json_data["strings"]) * 2 * 4)
	encoded_strs = []
	
	# Write header
	offset = header_space
	for string in json_data["strings"]:
		encoded_str = pokestr_encode(string)
		str_chars = len(encoded_str)
		str_bytes = str_chars * 2
		offset_bytes = str_bytes * 2
		
		# Odd 0xFFFF padding
		encoded_str += [ 0xFFFF ] * str_chars
		
		encoded_strs.append(encoded_str)
		
		file_write_u32_le(output, offset)
		file_write_u32_le(output, str_bytes)
		
		offset += offset_bytes
	
	# Write strings
	for encoded_str in encoded_strs:
		file_write_pokestring(output, encoded_str)
	
	# Pad to 0x1800
	while output.tell() < 0x1800:
		file_write_u16_le(output, 0xFFFF)


def unpack_bin(bin_file, output):
	result = {}
	bin_file.seek(0)
	magic_number = file_read_u32_le(bin_file)
	
	result["magic"] = magic_number;
	result["strings"] = []
	
	lowest_entry = 999999;
	while True:
		offset = file_read_u32_le(bin_file)
		length = file_read_u32_le(bin_file)
		
		prev_pos = bin_file.tell()
		bin_file.seek(offset)
		string_data = file_read_pokestring(bin_file)
		bin_file.seek(prev_pos)
		
		result["strings"].append(string_data)
		
		if offset < lowest_entry:
			lowest_entry = offset;
		
		if bin_file.tell() >= lowest_entry:
			break
	
	json_str = json.dumps(result, indent=4)
	if output is None:
		print(json_str)
	else:
		output.write(json_str.encode("ascii"))


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description=__doc__)
	
	parser.add_argument("operation", choices=["pack", "unpack"])
	parser.add_argument("input", type=argparse.FileType("rb"))
	parser.add_argument("-o", "--output", type=argparse.FileType("wb"))
	
	args = parser.parse_args()
	if args.operation == "unpack":
		unpack_bin(args.input, args.output)
	else:
		pack_json(args.input, args.output)
