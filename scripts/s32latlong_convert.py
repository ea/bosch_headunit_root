import json
import sys
import math
import simplekml
import datetime
import gpxpy
import gpxpy.gpx

def convert_s32latlong(n):
    f = (int(n) - 0.5 ) / 11930464.711111
    return f

f = open(sys.argv[1])
navs = json.load(f)
f.close()

gpx = gpxpy.gpx.GPX()
gpx_track = gpxpy.gpx.GPXTrack()
gpx.tracks.append(gpx_track)
gpx_segment = gpxpy.gpx.GPXTrackSegment()
gpx_track.segments.append(gpx_segment)



current_speed = 0.0
current_time = None
for nav in navs:
    print(current_speed)
    if nav['MethodName'] == 'aqNavInfoStatus':
        nav['s32Longitude'] = convert_s32latlong(nav['s32Longitude'])
        nav['s32Latitude'] = convert_s32latlong(nav['s32Latitude'])
        gpx_segment.points.append(gpxpy.gpx.GPXTrackPoint(nav['s32Latitude'], nav['s32Longitude'],time=current_time,speed=current_speed))

    if nav['MethodName'] == 'aqSetVehicleSpeed':
        current_speed = float(nav["VehicleSpeed"])
    if nav['MethodName'] == 'aqSetLocalTimeDate':
        current_time = datetime.datetime(int(nav['s16Year']),
                int(nav['u8Month']),
                int(nav['u8Day']),
                hour=int(nav['u8Hours']),
                minute=int(nav['u8Minutes']),
                second=int(nav['u8Seconds']))
        print(current_time)

f = open(sys.argv[1]+".gpx","w")
f.write(gpx.to_xml())
f.close()
