import Tkinter as tk
import random
import csv

def RGBToHTMLColor(rgb_tuple):
    """Converts a (r,g,b) tuple into #ffffff HTML color code for GUI"""
    hexcolor = '#%02x%02x%02x' % rgb_tuple
    return hexcolor


class App(tk.Tk):
    # GUI dimension setup
    def __init__(self, *args, **kwargs):
        tk.Tk.__init__(self, *args, **kwargs)
        self.canvas = tk.Canvas(self, width=600, height=800, borderwidth=0, highlightthickness=0)
        self.canvas.pack(side="top", fill="both", expand="true")
        self.rows = 4
        self.columns = 3
        self.cellwidth = 200
        self.cellheight = 200

        self.rect = {}
        self.oval = {}
        for column in range(20):
            for row in range(20):
                x1 = column*self.cellwidth
                y1 = row * self.cellheight
                x2 = x1 + self.cellwidth
                y2 = y1 + self.cellheight
                self.rect[row,column] = self.canvas.create_rectangle(x1,y1,x2,y2, fill="#fff", tags="rect")
                self.oval[row,column] = self.canvas.create_oval(x1+2,y1+2,x2-2,y2-2, fill="#fff", tags="oval")

        self.redraw(10) # poll for changes

    def redraw(self, delay):
#        self.canvas.itemconfig("rect", fill="#fff")
#        self.canvas.itemconfig("oval", fill="#fff")

	# for now, read from flatfile updated from the server
        try:
            with open("temp.txt","r") as f:
                reader = csv.reader(f)
                loc = next(reader)
            
#                print loc
		# update all of the colors based on the flatfile csv text line 
                for jj in range(12):
                    
                    sensor = loc[jj+2:len(loc):12]
#                    print sensor
                    
		    # This is normalized HTML color codes, 8 bit
                    for ii in range(len(sensor)):
                        sensor[ii] = int((float(sensor[ii]) / float(sensor[3])) * 255)
                    sensor = tuple(sensor[0:3])
#                    print sensor
                    row = jj % 4
                    col = jj / 4
                    item_id = self.oval[row,col]
                    self.canvas.itemconfig(item_id, fill=RGBToHTMLColor((sensor)))
                    
        except:
            pass
        f.close()
        self.after(delay, lambda: self.redraw(delay))

if __name__ == "__main__":
    app = App()
    app.mainloop()
