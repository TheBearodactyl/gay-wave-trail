local is_windows = package.config:sub(1, 1) == "\\"
local colors = {
	reset = "\27[0m",
	red = "\27[31m",
	green = "\27[32m",
	yellow = "\27[33m",
	magenta = "\27[35m",
	cyan = "\27[36m",
	bold = "\27[1m",
}

local enable = {
	git = false,
	pre_commit = false,
	cmake = false,
}

local function info(msg)
	print(colors.yellow .. "[INFO] " .. colors.reset .. msg)
end

local function success(msg)
	print(colors.green .. "[OK] " .. colors.reset .. msg)
end

local function err(msg)
	print(colors.red .. "[ERROR] " .. colors.reset .. msg)
end

local function warn(msg)
	print(colors.magenta .. "[WARN] " .. colors.reset .. msg)
end

local function join_args(args)
	local quoted = {}
	for _, a in ipairs(args) do
		if a:find(" ") then
			table.insert(quoted, '"' .. a .. '"')
		else
			table.insert(quoted, a)
		end
	end
	return table.concat(quoted, " ")
end

local function run_command(args, exit_on_fail)
	local cmd = join_args(args)
	info("Running: " .. cmd)
	local ok = os.execute(cmd)
	if ok ~= 0 then
		if exit_on_fail then
			err("Command failed: " .. cmd)
			os.exit(1)
		else
			warn("Command failed: " .. cmd)
		end
	end
end

local function print_help()
	print(colors.cyan .. colors.bold .. "Usage:" .. colors.reset .. " lua setup.lua [options]\n")
	print(colors.bold .. "Options:" .. colors.reset)
	print(
		"  "
			.. colors.yellow
			.. "--git"
			.. colors.reset
			.. "              Run "
			.. colors.cyan
			.. "'git pull'"
			.. colors.reset
	)
	print("  " .. colors.yellow .. "--pre-commit" .. colors.reset .. "       Link the git pre-commit hook")
	print("  " .. colors.yellow .. "--cmake" .. colors.reset .. "            Configure CMake")
	print("  " .. colors.yellow .. "-h, --help" .. colors.reset .. "         Show this help message\n")

	print(colors.bold .. "Examples:" .. colors.reset)
	print("  lua setup.lua")
	print("  lua setup.lua " .. colors.yellow .. "--git --cmake" .. colors.reset)
	print("  lua setup.lua " .. colors.yellow .. "--post-commit" .. colors.reset)
end

for _, arg in ipairs(arg) do
	if arg == "--git" then
		enable.git = true
	elseif arg == "--pre-commit" then
		enable.pre_commit = true
	elseif arg == "--cmake" then
		enable.cmake = true
	elseif arg == "--help" or arg == "-h" then
		print_help()
		os.exit(0)
	end
end

if enable.git then
	run_command({ "git", "pull" }, false)
	success("Repository updated.")
else
	warn("Skipping repo update")
end

local function dir_exists(path)
	if is_windows then
		local f = io.popen('if exist "' .. path .. '" (echo exists)')

		if not f then
			err("Check failed")
			os.exit(1)
		end

		local result = f:read("*l")
		f:close()
		return result == "exists"
	else
		local f = io.popen('[ -d "' .. path .. '" ] && echo exists')

		if not f then
			err("Check failed")
			os.exit(1)
		end

		local result = f:read("*l")
		f:close()
		return result == "exists"
	end
end

local function abs_path(path)
	local cmd
	if is_windows then
		cmd = 'for %I in ("' .. path .. '") do @echo %~fI'
	else
		cmd = 'realpath "' .. path .. '"'
	end
	local f = io.popen(cmd)

	if not f then
		err("Couldn't open file at path: " .. path)
		os.exit(1)
	end

	local out = f:read("*l")
	f:close()
	return out or path
end

if enable.pre_commit then
	if dir_exists(".git") then
		info(".git directory found, setting up pre-commit hook...")

		local src = abs_path("scripts/pre-commit.sh")
		local dest = abs_path(".git/hooks/pre-commit")

		if is_windows then
			os.execute(string.format('if exist "%s" del "%s"', dest, dest))
		else
			os.execute(string.format('rm -f "%s"', dest))
		end

		local link_cmd = { "ln", "-s", src, dest }

		local ok = os.execute(join_args(link_cmd))
		if ok ~= 0 then
			success("Symlink created: " .. dest .. " → " .. src)
			if not is_windows then
				os.execute('chmod +x "' .. src .. '"')
			end
		else
			warn("Failed to create symlink (run as Administrator or enable Developer Mode).")
		end
	else
		warn("No .git directory found, skipping hook setup.")
	end
else
	warn("Skipped pre-commit hook setup")
end

if enable.cmake then
	run_command({
		"cmake",
		"-S",
		".",
		"-B",
		"build",
		"-Wno-dev",
		"-G",
		"Ninja",
		"-DCMAKE_EXPORT_COMPILE_COMMANDS=ON",
	})

	success("CMake configuration complete.")
else
	warn("Skipping cmake configuration")
end

success("All tasks finished successfully.")
