import gi
gi.require_version('Gtk', '3.0')
from gi.repository import Gtk

ranking = [ ['java', 16.24, 1], ['C', 16.03, 1], ['Python', 9.84, 1], ['ja', 1, 1]]


sygnaly = { 'ondestroy': Gtk.main_quit }
builder = Gtk.Builder()
builder.add_from_file('wyklad.glade')

# model
model = builder.get_object('liststore1')
for p in ranking:
    model.append(p)


# kolumny
view = builder.get_object('treeview1')

r1 = Gtk.CellRendererText()
k1 = Gtk.TreeViewColumn("Jp", r1, text=0)
k1.set_sizing(Gtk.TreeViewColumnSizing.FIXED)
view.append_column(k1)

r2 = Gtk.CellRendererText()
k2 = Gtk.TreeViewColumn("popularnosc", r2, text=1)
k2.set_sizing(Gtk.TreeViewColumnSizing.FIXED)
view.append_column(k2)

def on_select(selektor):
    model, treeiter = selektor.get_selected()
    etyk = builder.get_object('label1') 
    if treeiter is not None:
         etyk.set_text("Wybrałeś " + model[treeiter][0])

sel = builder.get_object('treeview-selection1')
sel.connect('changed', on_select)
       


builder.connect_signals(sygnaly)

okno = builder.get_object('okno')

okno.show_all()

Gtk.main()

