od2trips -n districts.xml -d visum\koeln_based_DB.fma -o koeln_umlegung.trips.xml -v -b 0 -e 86400 --dismiss-loading-errors
duarouter -n koeln_umlegung.net.xml -t koeln_umlegung.trips.xml -o koeln_umlegung.rou.xml -v -e 3600 --continue-on-unbuild
