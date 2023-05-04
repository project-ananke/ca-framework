import subprocess
import platform
import argparse
import timeit

# Nice colors yoinked from Stack Overflow.
class termcc:
	HEADER = '\033[95m'
	OKBLUE = '\033[94m'
	OKCYAN = '\033[96m'
	OKGREEN = '\033[92m'
	WARNING = '\033[93m'
	FAIL = '\033[91m'
	ENDC = '\033[0m'
	BOLD = '\033[1m'
	UNDERLINE = '\033[4m'

def windows_compile(args):
	odin = "odin"
	source = 'source'
	output = 'build/ananke.exe'

	print('-- ' + termcc.HEADER + 'Windows Compilation' + termcc.ENDC)
	print('--- ' + termcc.HEADER + "Compilation Settings" + termcc.ENDC)
	print('---- ' + termcc.BOLD + 'odin executable: '+  termcc.ENDC + odin)
	print('---- ' + termcc.BOLD + 'source directory: ' + termcc.ENDC + source)
	print('---- ' + termcc.BOLD + 'output file: ' + termcc.ENDC + output)

	print('--- ' + termcc.HEADER + 'Compiling Ananke' + termcc.ENDC)

	command = 'odin build source -collection:ext=./engine/ext -collection:engine=./engine '
	if args.debug == True:
		command += '-debug '
	command += '-out:build/ananke.exe'

	print('---- ' + command)

	start_time = timeit.default_timer()
	res = subprocess.run(command)
	elapsed = 1000 * (timeit.default_timer() - start_time)

	if res.returncode == 0:
		print(f"--- {termcc.OKGREEN}SUCCESS{termcc.ENDC}: Compilation took {elapsed:.2f} ms.")
	else:
		print(f"--- {termcc.FAIL}FAILED{termcc.ENDC}: Compilation took {elapsed:.2f} ms.")

	# Maybe, in the future, we can use Python's filesystem module (if there is one)
	# to handle this for us. 
	print('-- ' + termcc.HEADER + 'Copying shaders to build directory' + termcc.ENDC)
	res = subprocess.run('xcopy /s /y /i engine\\shaders build\\shaders', stdout=subprocess.DEVNULL)

def windows_run(args):
	subprocess.run('build/ananke.exe')

def main():
	parser = argparse.ArgumentParser(prog='ananke_sys',
										description='ananke_sys is a tool to run Ananke tests and compilation.')
	parser.add_argument('command', nargs=1,
						help='Command to tell the system what to do.')
	parser.add_argument('--debug', dest='debug', action='store_true', default=False,
						help='Compiles with debug symbols for debugging.')

	args = parser.parse_args()

	if platform.system() == 'Windows':
		# Not sure exactly how the argparse module works, but it seems
		# to generate an array... which is kind of weird. This is why 
		# handmade software is peak because you actually understand how 
		# it works.
		if args.command[0] == 'build':
			windows_compile(args)
		elif args.command[0] == 'run':
			windows_run(args)


if __name__ == "__main__":
	main()