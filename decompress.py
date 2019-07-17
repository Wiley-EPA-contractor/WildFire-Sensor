# import statements if needed, update as timestamp print gets implemented
import sys
from tkinter import filedialog
from tkinter import *
from tkinter import simpledialog


#
class WildFireFile:
    def __init__(self, master):
        frame = Frame(master)
        frame.pack()
# this is the input file button
        self.fInputButton = Button(frame, text="File from Rockblock", command=self.inFile)
        self.fInputButton.pack(side=TOP)
# this is the decode file button
        self.fSendButton = Button(frame, text="Append Data to New/Existing File", command=self.send)
        self.fSendButton.pack(side=BOTTOM)
# this is the output file location/name button
        self.fOutputButton = Button(frame, text="File Output", command=self.outFile)
        self.fOutputButton.pack(side=BOTTOM)
# this function controls which input file is selected
    def inFile(self):
        # these are globals because I was kind of lazy instead of being really super nested with this program
        global file
        file = filedialog.askopenfilename(initialdir="/", title="Select file",
                                          filetypes=(("text files", "*.txt"), ("all files", "*.*"),
                                                     ("CSV Files", "*.csv")))
# this function controls which output file is selected
    def outFile(self):
        # these are globals because I was kind of lazy instead of being really super nested with this program
        global fout
        fout = filedialog.asksaveasfilename(initialdir="/", title="Select file",
                                            filetypes=(("jpeg files", "*.jpg"), ("all files", "*.*")))
# this function controls decodes the data and makes it csv compatible :D
    def send(self):
        with open(file) as f:
            read_data = f.readline()
            read = list(read_data)
            temp = str(ord(read[0]))
            humidity = str(ord(read[1]))
            particulate = str(ord(read[2]))

        msg = str(temp + "," + humidity + "," + particulate + '\n')
# gotta make sure to append not overwrite (That is what a+ does)! 
# also when opening files you had best close them afterwards or else.
        f0 = open(fout, 'a+')
        f0.write(msg)
        f0.close()


root = Tk()
b = WildFireFile(root)
root.mainloop()
