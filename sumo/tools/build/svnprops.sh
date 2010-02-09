find tests -regex ".*\.\(xml\|complex\|dfrouter\|duarouter\|jtrrouter\|netconvert\|netgen\|od2trips\|polyconvert\|sumo\|meso\|tools\|traci\)" -print0 | xargs -0 svn ps svn:eol-style LF

