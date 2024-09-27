import customtkinter as ctk
import serial
import threading

ser = serial.Serial('COM7', 115200, timeout=1)

WIDTH = 835
HEIGHT = 444

root = ctk.CTk()
root.title("Interfaz monitoreo de temp")
root.geometry(f"{WIDTH}x{HEIGHT}")  # Establecer el tamaño de la ventana principal

# Crear un recuadro principal para toda la ventana
main_frame = ctk.CTkFrame(root)
main_frame.pack(fill="both", expand=True)  # Empaquetar main_frame

display = []

for _ in range(3):
    obj = ctk.CTkFrame(
        main_frame,
        width=250,
        height=250,
        border_width=5,
        border_color="black"
    )
    display.append(obj)

display[0].place(x=20, y=20)
display[1].place(x=290, y=20)
display[2].place(x=565, y=20)

text = ["Temperatura 1", "Temperatura 2", "Temperatura 3"]

for i in range(3):
    label_text = ctk.CTkLabel(display[i], text=text[i], font=("Cyberpunk", 30, "bold"), text_color="#32CD32")
    label_text.place(relx=0.5, y=20, anchor="n")  # Posicionar el texto en el centro del display_1

texts_frames = []

for i in range(3):
    textfr = ctk.CTkFrame(
        display[i],
        width=210,
        height=150,
        fg_color="transparent"  # Fondo transparente
    )
    texts_frames.append(textfr)
    texts_frames[i].place(relx=0.5, y=70, anchor="n")

temp_vars = []
temp_labels = []

for i in range(3):
    temp = ctk.StringVar()
    temp.set("N. A.")
    temp_vars.append(temp)
    label_temp = ctk.CTkLabel(texts_frames[i], textvariable=temp_vars[i], font=("Neuropol", 60, "bold"))
    label_temp.place(relx=0.5, rely=0.5, anchor="center")
    temp_labels.append(label_temp)

def update_text(new_text, display):
    if new_text == 100:
        if display == 0:
            for x in range(3):
                temp_vars[x].set("N. A.")
                temp_labels[x].configure(text_color="#FFFFFF")
        if display == 1:
            temp_vars[0].set("N. A.")
            temp_labels[0].configure(text_color="#FFFFFF")
        if display == 2:
            temp_vars[1].set("N. A.")
            temp_labels[1].configure(text_color="#FFFFFF")
        if display == 3:
            temp_vars[2].set("N. A.")
            temp_labels[2].configure(text_color="#FFFFFF")
    else:
        if new_text <= 10:
            color = "#00FFFF"  # Cian
        elif 10 < new_text <= 20:
            color = "#0000FF"  # Azul
        elif 20 < new_text <= 30:
            color = "#FFFF00"  # Amarillo
        elif 30 < new_text <= 40:
            color = "#FFA500"  # Naranja
        elif new_text > 40:
            color = "#FF0000"  # Rojo
        new_text = str(new_text)
        temp_vars[display-1].set(new_text + " C°")
        temp_labels[display - 1].configure(text_color=color)

def read_from_arduino():
    while True:
        if ser.in_waiting > 0:
            data_received = ser.readline().decode('utf-8').strip()
            print(data_received)
            data_parts = data_received.split(' ')
            num_display = int(data_parts[-1])
            temperature = int(data_parts[0])
            update_text(temperature, num_display)

serial_thread = threading.Thread(target=read_from_arduino, daemon=True)
serial_thread.start()

# Mostrar la ventana
root.mainloop()