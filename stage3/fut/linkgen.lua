#!/usr/bin/env lua
--[[

--- trigger warning: brain damage ---

BOGUS DYNAMIC LINKING v69.420 by LIZZY FLECKENSTEIN

How this works:
⠀⠀⠀⠀⠀⢰⡿⠋⠁⠀⠀⠈⠉⠙⠻⣷⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⢀⣿⠇⠀⢀⣴⣶⡾⠿⠿⠿⢿⣿⣦⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⣀⣀⣸⡿⠀⠀⢸⣿⣇⠀⠀⠀⠀⠀⠀⠙⣷⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⣾⡟⠛⣿⡇⠀⠀⢸⣿⣿⣷⣤⣤⣤⣤⣶⣶⣿⠇⠀⠀⠀⠀⠀⠀⠀⣀⠀⠀
⢀⣿⠀⢀⣿⡇⠀⠀⠀⠻⢿⣿⣿⣿⣿⣿⠿⣿⡏⠀⠀⠀⠀⢴⣶⣶⣿⣿⣿⣆
⢸⣿⠀⢸⣿⡇⠀⠀⠀⠀⠀⠈⠉⠁⠀⠀⠀⣿⡇⣀⣠⣴⣾⣮⣝⠿⠿⠿⣻⡟
⢸⣿⠀⠘⣿⡇⠀⠀⠀⠀⠀⠀⠀⣠⣶⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠁⠉⠀
⠸⣿⠀⠀⣿⡇⠀⠀⠀⠀⠀⣠⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠟⠉⠀⠀⠀⠀
⠀⠻⣷⣶⣿⣇⠀⠀⠀⢠⣼⣿⣿⣿⣿⣿⣿⣿⣛⣛⣻⠉⠁⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⢸⣿⠀⠀⠀⢸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⢸⣿⣀⣀⣀⣼⡿⢿⣿⣿⣿⣿⣿⡿⣿⣿⡿

Amogus sus 😳 ((impassta))

I FUCKING HATE TEH ANTICRIST!!!!
WE GOT YOU SURROUNDEND, USE DYNAMIC LINKING

:trollface:

]]

-- in
local hdr_kern = io.open("sys_kern.h", "r")
-- out
local hdr_user = io.open("sys_user.h", "w")
local src_kern = io.open("sys_kern.c", "w")
local src_user = io.open("sys_user.c", "w")

local count = 0
local calltable = "0x500"

local body_kern = ""
local body_user = ""

src_kern:write("#include <sys_kern.h>\n")
src_user:write("#include <sys_user.h>\n\n")

local function process_line(line)
	if line == "" or line:sub(1, 1) == "\t" then
		return
	end

	local f, t = line:find("[%w_]+%(")

	if not f then
		return
	end

	local start, mid, fin = line:sub(1, f-1), line:sub(f, t-1), line:sub(t)

	if mid == "__attribute__" or mid == "link" then
		return
	end

	local decl = start .. "(*" .. mid .. ")" .. fin .. "\n"

	hdr_user:write("extern " .. decl)
	src_user:write(decl)

	body_kern = body_kern .. "\tcalltable[" .. count .. "] = &" .. mid .. ";\n"
	body_user = body_user .. "\t" .. mid .. " = calltable[" .. count .. "];\n"

	count = count + 1

	return true
end

while true do
	local line = hdr_kern:read()

	if not line then
		break
	end

	if not process_line(line) then
		hdr_user:write(line .. "\n")
	end
end

local function write_link(f, content)
	f:write("\nvoid link()\n{\n")
	f:write("\tvoid **calltable = " .. calltable .. ";\n\n")
	f:write(content)
	f:write("}\n")
end

write_link(src_kern, body_kern)
write_link(src_user, body_user)

hdr_kern:close()
hdr_user:close()
src_kern:close()
src_user:close()
