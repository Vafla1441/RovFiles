import gi
gi.require_version('Gst', '1.0')
from gi.repository import Gst, GLib

Gst.init(None)

pipeline_str = (
    'rtspsrc location="rtsp://root:12345@192.168.1.6/stream=0" latency=0 ! '
    'rtph264depay ! avdec_h264 ! videoconvert ! autovideosink sync=false'
)

pipeline = Gst.parse_launch(pipeline_str)
pipeline.set_state(Gst.State.PLAYING)

loop = GLib.MainLoop()
try:
    loop.run()
except KeyboardInterrupt:
    pipeline.set_state(Gst.State.NULL)
    loop.quit()
