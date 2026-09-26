import argparse

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description=__doc__)
	parser.add_argument("input", type=argparse.FileType("rb+"))
	
	args = parser.parse_args()
	
	input = args.input
	
	input.seek(0x15E)
	input.write(b'\xD6\xA8')
	
