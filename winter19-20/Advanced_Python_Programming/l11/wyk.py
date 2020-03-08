import gi
gi.require_version('Gtk', '3.0')
from gi.repository import Gtk


class Rysownik:
    def __init__(self):
        builder = Gtk.Builder()
        builder.add_from_file('wyklad.glade')
        self.window = builder.get_object('okno')
        builder.connect_signals(self)
    
    def on_window_destroy(self, widget, data=None):  pass
    
    def koniec(self, widget):  pass


rysunek = Rysownik()
rysunek.window.show()
Gtk.main()
