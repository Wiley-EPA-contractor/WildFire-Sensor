# import statements if needed
import sys
from tkinter import filedialog
from tkinter import *
from tkinter import simpledialog


#
class WildFireFile:
    def __init__(self, master):
        frame = Frame(master)
        frame.pack()

        self.fInputButton = Button(frame, text="File from Rockblock", command=self.inFile)
        self.fInputButton.pack(side=TOP)

        self.fSendButton = Button(frame, text="Append Data to New/Existing File", command=self.send)
        self.fSendButton.pack(side=BOTTOM)

        self.fOutputButton = Button(frame, text="File Output", command=self.outFile)
        self.fOutputButton.pack(side=BOTTOM)

    def inFile(self):
        global file
        file = filedialog.askopenfilename(initialdir="/", title="Select file",
                                          filetypes=(("text files", "*.txt"), ("all files", "*.*"),
                                                     ("CSV Files", "*.csv")))

    def outFile(self):
        global fout
        fout = filedialog.asksaveasfilename(initialdir="/", title="Select file",
                                            filetypes=(("jpeg files", "*.jpg"), ("all files", "*.*")))

    def send(self):
        with open(file) as f:
            read_data = f.readline()
            read = list(read_data)
            temp = str(ord(read[0]))
            humidity = str(ord(read[1]))
            particulate = str(ord(read[2]))

        msg = str(temp + "," + humidity + "," + particulate + '\n')

        f0 = open(fout, 'a+')
        f0.write(msg)
        f0.close()


root = Tk()
b = WildFireFile(root)
root.mainloop()
