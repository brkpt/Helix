require 'io'
require 'getopt'

local getopt = require 'getopt'

options = getopt.makeOptions {
	getopt.Option {{"input"}, "Input file", "Opt", "INPUTFILE"},
	getopt.Option {{"output"}, "Output file", "Opt", "OUTPUTFILE" },
	getopt.Option {{"help"}, "Display help"},
}

nonOpts, opts, errors = getopt.getOpt (arg, options)
if #errors ~= 0 or type(nonOpts) ~= 'table' or opts.help then
	print (table.concat (errors, "\n") .. "\n" ..
	getopt.usageInfo ("Usage: ProcessGateway.lua [OPTION...]", options))
	os.exit(1)
end

if opts.input == nil or opts.output == nil then
	print('Both input and output files are required '.. '\n' ..
		getopt.usageInfo('Usage: projfiles.lua [OPTION...]', options))
	os.exit(1)
end

-- Load the .lua script that has all the files listed in it
chunk,err = loadfile(opts.input[1])
if chunk == nil then
	print("error parsing")
	print(err)
	error()
end

ret, err = pcall(chunk)
if not ret then
	print("error running")
	print(err)
	error()
end

f = io.open(opts.output[1],'w')
if f == nil then
	print('error opening filelist')
	error()
end

for k,v in pairs(Projects) do
	for _,file in pairs(v.Sources) do
		f:write(file..'\n')
	end
end

f:close()


