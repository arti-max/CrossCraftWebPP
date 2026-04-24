# model_editor_fixed.py
import tkinter as tk
from tkinter import ttk, filedialog, messagebox
from PIL import Image, ImageTk, ImageDraw
import json
import os
import math
import numpy as np

# ---------- Data structures ----------
class TextureRegion:
    def __init__(self, name, x, y, w, h):
        self.name = name
        self.x = x
        self.y = y
        self.w = w
        self.h = h

    def to_dict(self):
        return {"name": self.name, "x": self.x, "y": self.y, "w": self.w, "h": self.h}

    @staticmethod
    def from_dict(d):
        return TextureRegion(d["name"], d["x"], d["y"], d["w"], d["h"])

class CustomVariable:
    def __init__(self, name, initial_value="0.0f"):
        self.name = name
        self.initial = initial_value

    def to_dict(self):
        return {"name": self.name, "initial": self.initial}

    @staticmethod
    def from_dict(d):
        return CustomVariable(d["name"], d["initial"])

class CubeDef:
    def __init__(self, name):
        self.name = name
        self.pos = [0.0, 0.0, 0.0]   # локальный угол (для addBox)
        self.size = [1, 1, 1]        # размеры (w, h, d)
        self.pivot = [0.0, 0.0, 0.0] # точка вращения (setPosition)
        self.mirror = False
        self.region = None           # имя текстурного региона (один на весь куб)
        self.anim_pitch = ""
        self.anim_yaw = ""
        self.anim_roll = ""

    def to_dict(self):
        return {
            "name": self.name,
            "pos": self.pos,
            "size": self.size,
            "pivot": self.pivot,
            "mirror": self.mirror,
            "region": self.region,
            "anim_pitch": self.anim_pitch,
            "anim_yaw": self.anim_yaw,
            "anim_roll": self.anim_roll
        }

    @staticmethod
    def from_dict(d):
        c = CubeDef(d["name"])
        c.pos = d["pos"]
        c.size = d["size"]
        c.pivot = d["pivot"]
        c.mirror = d.get("mirror", False)
        c.region = d.get("region")
        c.anim_pitch = d.get("anim_pitch", "")
        c.anim_yaw = d.get("anim_yaw", "")
        c.anim_roll = d.get("anim_roll", "")
        return c

class ModelData:
    def __init__(self):
        self.name = "model"
        self.texture_path = ""
        self.regions = []
        self.cubes = []
        self.custom_vars = []

    def to_dict(self):
        return {
            "name": self.name,
            "texture": self.texture_path,
            "regions": [r.to_dict() for r in self.regions],
            "cubes": [c.to_dict() for c in self.cubes],
            "custom_vars": [v.to_dict() for v in self.custom_vars]
        }

    def save(self, path):
        with open(path, "w") as f:
            json.dump(self.to_dict(), f, indent=2)

    def load(self, path):
        with open(path, "r") as f:
            d = json.load(f)
        self.name = d.get("name", "model")
        self.texture_path = d["texture"]
        self.regions = [TextureRegion.from_dict(r) for r in d["regions"]]
        self.cubes = [CubeDef.from_dict(c) for c in d["cubes"]]
        self.custom_vars = [CustomVariable.from_dict(v) for v in d.get("custom_vars", [])]

# ---------- Texture Editor ----------
class TextureEditor(tk.Canvas):
    def __init__(self, parent, app, **kwargs):
        super().__init__(parent, bg="#333333", **kwargs)
        self.app = app
        self.scale = 4.0
        self.offset_x = 0
        self.offset_y = 0
        self.img = None
        self.tk_img = None
        self.drag_start = None
        self.select_start = None
        self.current_rect = None
        self.region_rects = {}

        self.bind("<ButtonPress-1>", self.on_mouse_down)
        self.bind("<B1-Motion>", self.on_mouse_drag)
        self.bind("<ButtonRelease-1>", self.on_mouse_up)
        self.bind("<MouseWheel>", self.on_mouse_wheel)
        self.bind("<ButtonPress-3>", self.on_pan_start)
        self.bind("<B3-Motion>", self.on_pan_move)
        self.bind("<ButtonRelease-3>", self.on_pan_end)
        self.bind("<Double-Button-1>", self.on_double_click)

    def set_image(self, pil_image):
        self.img = pil_image
        self.reset_view()
        self.redraw()

    def reset_view(self):
        if self.img:
            w, h = self.img.size
            self.scale = min(self.winfo_width()/w, self.winfo_height()/h, 4.0)
            self.offset_x = (self.winfo_width() - w*self.scale)/2
            self.offset_y = (self.winfo_height() - h*self.scale)/2
        else:
            self.scale = 4.0
            self.offset_x = 0
            self.offset_y = 0

    def redraw(self):
        self.delete("all")
        if not self.img:
            return
        w, h = self.img.size
        scaled_w = int(w * self.scale)
        scaled_h = int(h * self.scale)
        resized = self.img.resize((scaled_w, scaled_h), Image.NEAREST)
        self.tk_img = ImageTk.PhotoImage(resized)
        self.create_image(self.offset_x, self.offset_y, anchor=tk.NW, image=self.tk_img, tags="texture")
        for region in self.app.model.regions:
            self.draw_region(region)

    def draw_region(self, region):
        x0 = region.x * self.scale + self.offset_x
        y0 = region.y * self.scale + self.offset_y
        x1 = (region.x + region.w) * self.scale + self.offset_x
        y1 = (region.y + region.h) * self.scale + self.offset_y
        rect_id = self.create_rectangle(x0, y0, x1, y1, outline="#00ff00", width=2, tags="region")
        text_id = self.create_text(x0+2, y0+2, anchor=tk.NW, text=region.name, fill="#ffff00", font=("Arial", 8, "bold"))
        self.region_rects[region.name] = (rect_id, text_id)

    def canvas_to_image_coords(self, canvas_x, canvas_y):
        ix = (canvas_x - self.offset_x) / self.scale
        iy = (canvas_y - self.offset_y) / self.scale
        if self.img:
            ix = max(0, min(self.img.width-1, int(round(ix))))
            iy = max(0, min(self.img.height-1, int(round(iy))))
        return ix, iy

    def on_mouse_down(self, event):
        if not self.img: return
        self.select_start = (event.x, event.y)
        if self.current_rect:
            self.delete(self.current_rect)
            self.current_rect = None

    def on_mouse_drag(self, event):
        if not self.select_start: return
        if self.current_rect:
            self.coords(self.current_rect, self.select_start[0], self.select_start[1], event.x, event.y)
        else:
            self.current_rect = self.create_rectangle(
                self.select_start[0], self.select_start[1], event.x, event.y,
                outline="red", width=2, dash=(4,2)
            )

    def on_mouse_up(self, event):
        if not self.select_start or not self.current_rect: return
        x0, y0 = self.select_start
        x1, y1 = event.x, event.y
        self.delete(self.current_rect)
        self.current_rect = None
        self.select_start = None

        if abs(x1-x0) < 5 or abs(y1-y0) < 5:
            return

        ix0, iy0 = self.canvas_to_image_coords(x0, y0)
        ix1, iy1 = self.canvas_to_image_coords(x1, y1)
        if ix0 > ix1: ix0, ix1 = ix1, ix0
        if iy0 > iy1: iy0, iy1 = iy1, iy0
        w = ix1 - ix0 + 1
        h = iy1 - iy0 + 1
        if w < 2 or h < 2: return
        self._create_region(ix0, iy0, w, h)

    def _create_region(self, x, y, w, h, suggested_name=None):
        if suggested_name is None:
            suggested_name = f"region_{len(self.app.model.regions)+1}"
        dialog = tk.Toplevel(self)
        dialog.title("New Region")
        ttk.Label(dialog, text="Region name:").pack(padx=10, pady=5)
        name_var = tk.StringVar(value=suggested_name)
        entry = ttk.Entry(dialog, textvariable=name_var)
        entry.pack(padx=10, pady=5)
        entry.focus()
        entry.select_range(0, tk.END)
        def on_ok():
            name = name_var.get().strip()
            if not name:
                messagebox.showerror("Error", "Name required")
                return
            if any(r.name == name for r in self.app.model.regions):
                messagebox.showerror("Error", "Region name must be unique")
                return
            region = TextureRegion(name, x, y, w, h)
            self.app.model.regions.append(region)
            self.draw_region(region)
            self.app.update_region_list()
            self.app.update_preview()
            dialog.destroy()
        ttk.Button(dialog, text="OK", command=on_ok).pack(pady=5)
        dialog.transient(self)
        dialog.grab_set()

    def edit_region(self, region):
        dialog = tk.Toplevel(self)
        dialog.title(f"Edit Region: {region.name}")
        ttk.Label(dialog, text="Name:").grid(row=0, column=0, padx=5, pady=5, sticky="e")
        name_var = tk.StringVar(value=region.name)
        ttk.Entry(dialog, textvariable=name_var).grid(row=0, column=1, padx=5, pady=5)

        ttk.Label(dialog, text="X:").grid(row=1, column=0, sticky="e")
        x_var = tk.IntVar(value=region.x)
        ttk.Entry(dialog, textvariable=x_var, width=8).grid(row=1, column=1, sticky="w")
        ttk.Label(dialog, text="Y:").grid(row=2, column=0, sticky="e")
        y_var = tk.IntVar(value=region.y)
        ttk.Entry(dialog, textvariable=y_var, width=8).grid(row=2, column=1, sticky="w")
        ttk.Label(dialog, text="Width:").grid(row=3, column=0, sticky="e")
        w_var = tk.IntVar(value=region.w)
        ttk.Entry(dialog, textvariable=w_var, width=8).grid(row=3, column=1, sticky="w")
        ttk.Label(dialog, text="Height:").grid(row=4, column=0, sticky="e")
        h_var = tk.IntVar(value=region.h)
        ttk.Entry(dialog, textvariable=h_var, width=8).grid(row=4, column=1, sticky="w")

        def on_save():
            new_name = name_var.get().strip()
            if not new_name:
                messagebox.showerror("Error", "Name required")
                return
            if new_name != region.name and any(r.name == new_name for r in self.app.model.regions):
                messagebox.showerror("Error", "Name already exists")
                return
            old_name = region.name
            region.name = new_name
            region.x = x_var.get()
            region.y = y_var.get()
            region.w = w_var.get()
            region.h = h_var.get()
            self.redraw()
            self.app.update_region_list()
            if old_name != new_name:
                for cube in self.app.model.cubes:
                    if cube.region == old_name:
                        cube.region = new_name
            self.app.update_preview()
            dialog.destroy()
        ttk.Button(dialog, text="Save", command=on_save).grid(row=5, columnspan=2, pady=10)
        dialog.transient(self)
        dialog.grab_set()

    def on_double_click(self, event):
        ix, iy = self.canvas_to_image_coords(event.x, event.y)
        for region in self.app.model.regions:
            if region.x <= ix < region.x+region.w and region.y <= iy < region.y+region.h:
                self.edit_region(region)
                break

    def on_mouse_wheel(self, event):
        if not self.img: return
        scale_factor = 1.1 if event.delta > 0 else 0.9
        new_scale = self.scale * scale_factor
        if 0.5 <= new_scale <= 16:
            cx, cy = event.x, event.y
            self.offset_x = cx - (cx - self.offset_x) * (new_scale / self.scale)
            self.offset_y = cy - (cy - self.offset_y) * (new_scale / self.scale)
            self.scale = new_scale
            self.redraw()

    def on_pan_start(self, event):
        self.drag_start = (event.x, event.y)
        self.config(cursor="fleur")

    def on_pan_move(self, event):
        if self.drag_start:
            dx = event.x - self.drag_start[0]
            dy = event.y - self.drag_start[1]
            self.offset_x += dx
            self.offset_y += dy
            self.drag_start = (event.x, event.y)
            self.redraw()

    def on_pan_end(self, event):
        self.drag_start = None
        self.config(cursor="")

    def delete_region(self, name):
        if name in self.region_rects:
            rect_id, text_id = self.region_rects.pop(name)
            self.delete(rect_id)
            self.delete(text_id)

    def clear_regions(self):
        for name in list(self.region_rects.keys()):
            self.delete_region(name)
        self.region_rects.clear()

# ---------- 3D Preview with PyOpenGL ----------
try:
    from OpenGL.GL import *
    from OpenGL.GLU import *
    import pyopengltk
    OPENGL_AVAILABLE = True
except ImportError:
    OPENGL_AVAILABLE = False
    print("PyOpenGL or pyopengltk not installed. 3D preview disabled.")

if OPENGL_AVAILABLE:
    class GLPreview(pyopengltk.OpenGLFrame):
        def __init__(self, parent, app, **kwargs):
            super().__init__(parent, **kwargs)
            self.app = app
            self.rot_x = 30
            self.rot_y = 45
            self.zoom = -20
            self.offset_x = 0.0
            self.offset_y = 0.0
            self.offset_z = 0.0
            self.last_mouse = None
            self.panning = False
            self.texture_id = None
            self.display_list = None
            self.bind("<ButtonPress-1>", self.on_mouse_down)
            self.bind("<B1-Motion>", self.on_mouse_drag)
            self.bind("<ButtonRelease-1>", self.on_mouse_up)
            self.bind("<ButtonPress-3>", self.on_pan_start)
            self.bind("<B3-Motion>", self.on_pan_move)
            self.bind("<ButtonRelease-3>", self.on_pan_end)
            self.bind("<MouseWheel>", self.on_mouse_wheel)
            self.anim_time = 0.0
            self.anim_speed = 1.0
            self.anim_tick = 0.0
            self.anim_playing = False
            self.anim_custom_vars = {} 
            
            ctrl = ttk.Frame(self)
            ctrl.pack(side=tk.BOTTOM, fill=tk.X, padx=5, pady=5)
            ttk.Label(ctrl, text="Time:").pack(side=tk.LEFT)
            self.time_scale = ttk.Scale(ctrl, from_=0, to=10, orient=tk.HORIZONTAL, command=self.on_time_slider)
            self.time_scale.pack(side=tk.LEFT, fill=tk.X, expand=True, padx=5)
            self.play_btn = ttk.Button(ctrl, text="▶", width=3, command=self.toggle_play)
            self.play_btn.pack(side=tk.LEFT, padx=2)
            ttk.Button(ctrl, text="Reset", command=self.reset_anim).pack(side=tk.LEFT, padx=2)
            ttk.Label(ctrl, text="Speed:").pack(side=tk.LEFT, padx=(10,0))
            self.speed_var = tk.DoubleVar(value=1.0)
            ttk.Spinbox(ctrl, from_=0.1, to=5.0, increment=0.1, textvariable=self.speed_var, width=5, command=self.update_speed).pack(side=tk.LEFT)

        def update_speed(self):
            self.anim_speed = self.speed_var.get()
            
        def on_time_slider(self, val):
            self.anim_time = float(val)
            self.anim_tick = self.anim_time * 20.0  # примерное соответствие tick ~ time*20
            self.compile_model()  # перекомпилируем с новыми углами
            self.redraw()

        def toggle_play(self):
            self.anim_playing = not self.anim_playing
            self.play_btn.config(text="⏸" if self.anim_playing else "▶")
            if self.anim_playing:
                self.animate()

        def animate(self):
            if not self.anim_playing:
                return
            self.anim_time += 0.05 * self.anim_speed
            if self.anim_time > 10.0:
                self.anim_time = 0.0
            self.time_scale.set(self.anim_time)
            self.anim_tick = self.anim_time * 20.0
            self.compile_model()
            self.redraw()
            self.after(50, self.animate)

        def reset_anim(self):
            self.anim_time = 0.0
            self.anim_tick = 0.0
            self.time_scale.set(0.0)
            self.compile_model()
            self.redraw()
        
        def initgl(self):
            glClearColor(0.2, 0.2, 0.2, 1.0)
            glEnable(GL_DEPTH_TEST)
            glEnable(GL_TEXTURE_2D)
            glDisable(GL_CULL_FACE)  # оставляем для диагностики
            self.load_texture()
            self.compile_model()

        def load_texture(self):
            if not self.app.texture_img:
                return
            img = self.app.texture_img.convert("RGBA")
            img_data = np.array(img, dtype=np.uint8)
            w, h = img.size
            self.texture_id = glGenTextures(1)
            glBindTexture(GL_TEXTURE_2D, self.texture_id)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img_data)

        def compile_model(self):
            if self.display_list:
                glDeleteLists(self.display_list, 1)
            self.display_list = glGenLists(1)
            glNewList(self.display_list, GL_COMPILE)
            
            # Подготавливаем контекст для eval
            import math as m
            context = {
                "time": self.anim_time,
                "speed": self.anim_speed,
                "tick": self.anim_tick,
                "headYRot": 0.0,  # для совместимости
                "headXRot": 0.0,
                "M_PI": m.pi,
                "cos": m.cos,
                "sin": m.sin,
                "fabs": m.fabs,
                "sqrt": m.sqrt,
                # кастомные переменные
            }
            for var in self.app.model.custom_vars:
                context[var.name] = self.anim_custom_vars.get(var.name, float(var.initial.rstrip('f')))
            
            for cube in self.app.model.cubes:
                # Вычисляем углы
                pitch = yaw = roll = 0.0
                try:
                    if cube.anim_pitch:
                        pitch = eval(cube.anim_pitch, {"__builtins__": None}, context)
                    if cube.anim_yaw:
                        yaw = eval(cube.anim_yaw, {"__builtins__": None}, context)
                    if cube.anim_roll:
                        roll = eval(cube.anim_roll, {"__builtins__": None}, context)
                except Exception as e:
                    print(f"Error evaluating animation for {cube.name}: {e}")
                self.draw_cube(cube, pitch, yaw, roll)
            # Pivot
            glPointSize(8.0)
            glBegin(GL_POINTS)
            glColor3f(1.0, 0.0, 0.0)
            for cube in self.app.model.cubes:
                glVertex3f(cube.pivot[0], cube.pivot[1], cube.pivot[2])
            glEnd()
            glEndList()

        def draw_cube(self, cube, pitch, yaw, roll):
            tex_img = self.app.texture_img
            tex_w, tex_h = tex_img.size

            region = None
            if cube.region:
                region = next((r for r in self.app.model.regions if r.name == cube.region), None)

            if region is None:
                glDisable(GL_TEXTURE_2D)
                glColor3f(1.0, 0.0, 1.0)
            else:
                glEnable(GL_TEXTURE_2D)
                glBindTexture(GL_TEXTURE_2D, self.texture_id)
                glColor3f(1.0, 1.0, 1.0)

            u0 = region.x if region else 0
            v0 = region.y if region else 0
            w = cube.size[0]
            h = cube.size[1]
            d = cube.size[2]

            def norm_u(u): return u / tex_w
            def norm_v(v): return v / tex_h

            # UV quads (u_start, v_start, u_end, v_end)
            quads_uv = [
                (u0 + d + w, v0 + d, u0 + d + w + d, v0 + d + h),  # right
                (u0, v0 + d, u0 + d, v0 + d + h),                  # left
                (u0 + d, v0, u0 + d + w, v0 + d),                  # top
                (u0 + d + w, v0, u0 + d + w + w, v0 + d),          # bottom
                (u0 + d, v0 + d, u0 + d + w, v0 + d + h),          # front
                (u0 + d + w + d, v0 + d, u0 + d + w + d + w, v0 + d + h)  # back
            ]

            px, py, pz = cube.pos
            corners = [
                (0, 0, 0), (w, 0, 0), (w, h, 0), (0, h, 0),
                (0, 0, d), (w, 0, d), (w, h, d), (0, h, d)
            ]

            faces = [
                (5, 1, 2, 6),  # right
                (0, 4, 7, 3),  # left
                (5, 4, 0, 1),  # top
                (2, 3, 7, 6),  # bottom
                (1, 0, 3, 2),  # front
                (4, 5, 6, 7)   # back
            ]

            glPushMatrix()
            glTranslatef(cube.pivot[0], cube.pivot[1], cube.pivot[2])
            
            if roll != 0.0:
                glRotatef(roll * 57.29578, 0, 0, 1)
            if yaw != 0.0:
                glRotatef(yaw * 57.29578, 0, 1, 0)
            if pitch != 0.0:
                glRotatef(pitch * 57.29578, 1, 0, 0)

            for i, face_idx in enumerate(faces):
                u1, v1, u2, v2 = quads_uv[i]
                # Инвертируем U для правой и левой граней (возможно, исправит зеркальность)
                if i == 0 or i == 1:
                    u1, u2 = u2, u1
                u1_n, v1_n = norm_u(u1), norm_v(v1)
                u2_n, v2_n = norm_u(u2), norm_v(v2)

                glBegin(GL_QUADS)
                pts = [corners[idx] for idx in face_idx]
                pts = [(x+px, y+py, z+pz) for (x,y,z) in pts]
                glTexCoord2f(u1_n, v1_n); glVertex3f(*pts[0])
                glTexCoord2f(u2_n, v1_n); glVertex3f(*pts[1])
                glTexCoord2f(u2_n, v2_n); glVertex3f(*pts[2])
                glTexCoord2f(u1_n, v2_n); glVertex3f(*pts[3])
                glEnd()

            glPopMatrix()

            if region is None:
                glEnable(GL_TEXTURE_2D)
                glColor3f(1.0, 1.0, 1.0)

        def compute_center(self):
            if not self.app.model.cubes:
                return (0.0, 0.0, 0.0)
            cx = sum(c.pivot[0] for c in self.app.model.cubes) / len(self.app.model.cubes)
            cy = sum(c.pivot[1] for c in self.app.model.cubes) / len(self.app.model.cubes)
            cz = sum(c.pivot[2] for c in self.app.model.cubes) / len(self.app.model.cubes)
            return (cx, cy, cz)

        def redraw(self):
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
            glLoadIdentity()
            gluPerspective(45, self.width/self.height, 0.1, 100.0)
            
            center = self.compute_center()
            glTranslatef(self.offset_x, self.offset_y, self.zoom + self.offset_z)
            glTranslatef(-center[0], -center[1], -center[2])
            glRotatef(self.rot_x, 1, 0, 0)
            glRotatef(self.rot_y, 0, 1, 0)
            glTranslatef(center[0], center[1], center[2])

            # Инверсия Y как в игре
            glScalef(1.0, -1.0, 1.0)

            if self.display_list:
                glCallList(self.display_list)
            else:
                self.draw_axes()

            self.tkSwapBuffers()

        def draw_axes(self):
            glBegin(GL_LINES)
            glColor3f(1,0,0); glVertex3f(0,0,0); glVertex3f(10,0,0)
            glColor3f(0,1,0); glVertex3f(0,0,0); glVertex3f(0,10,0)
            glColor3f(0,0,1); glVertex3f(0,0,0); glVertex3f(0,0,10)
            glEnd()

        # Управление мышью
        def on_mouse_down(self, event):
            self.last_mouse = (event.x, event.y)

        def on_mouse_drag(self, event):
            if self.last_mouse and not self.panning:
                dx = event.x - self.last_mouse[0]
                dy = event.y - self.last_mouse[1]
                self.rot_y += dx * 0.5   # инвертировано как вы хотели
                self.rot_x += dy * 0.5   # прямое
                self.last_mouse = (event.x, event.y)
                self.redraw()

        def on_mouse_up(self, event):
            self.last_mouse = None

        def on_pan_start(self, event):
            self.panning = True
            self.last_mouse = (event.x, event.y)

        def on_pan_move(self, event):
            if self.last_mouse and self.panning:
                dx = event.x - self.last_mouse[0]
                dy = event.y - self.last_mouse[1]
                self.offset_x += dx * 0.05
                self.offset_y -= dy * 0.05
                self.last_mouse = (event.x, event.y)
                self.redraw()

        def on_pan_end(self, event):
            self.panning = False
            self.last_mouse = None

        def on_mouse_wheel(self, event):
            self.zoom += event.delta / 120.0
            self.zoom = max(-50, min(-5, self.zoom))
            self.redraw()

        def refresh(self):
            self.load_texture()
            self.compile_model()
            self.redraw()
else:
    class GLPreview(tk.Frame):
        def __init__(self, parent, app, **kwargs):
            super().__init__(parent, **kwargs)
            label = tk.Label(self, text="OpenGL not available.\nInstall pyopengltk and PyOpenGL.")
            label.pack(expand=True)
        def refresh(self): pass

# ---------- Main Application ----------
class ModelEditorApp:
    def __init__(self, root):
        self.root = root
        self.root.title("CC - Online: Model Editor")
        self.model = ModelData()
        self.texture_img = None
        self.create_menu()
        self.create_ui()

    def create_menu(self):
        menubar = tk.Menu(self.root)
        file_menu = tk.Menu(menubar, tearoff=0)
        file_menu.add_command(label="Load Texture", command=self.load_texture)
        file_menu.add_command(label="Save Model", command=self.save_model)
        file_menu.add_command(label="Load Model", command=self.load_model)
        file_menu.add_separator()
        file_menu.add_command(label="Generate .hpp", command=self.generate_hpp)
        menubar.add_cascade(label="File", menu=file_menu)
        self.root.config(menu=menubar)

    def create_ui(self):
        paned = ttk.PanedWindow(self.root, orient=tk.HORIZONTAL)
        paned.pack(fill=tk.BOTH, expand=True)

        left_frame = ttk.Frame(paned)
        paned.add(left_frame, weight=2)

        name_frame = ttk.Frame(left_frame)
        name_frame.pack(fill=tk.X, padx=5, pady=5)
        ttk.Label(name_frame, text="Model Name:").pack(side=tk.LEFT)
        self.model_name_var = tk.StringVar(value=self.model.name)
        name_entry = ttk.Entry(name_frame, textvariable=self.model_name_var)
        name_entry.pack(side=tk.LEFT, fill=tk.X, expand=True, padx=5)
        name_entry.bind("<KeyRelease>", lambda e: setattr(self.model, 'name', self.model_name_var.get()))

        tex_frame = ttk.LabelFrame(left_frame, text="Texture", padding=2)
        tex_frame.pack(fill=tk.BOTH, expand=True)
        self.tex_editor = TextureEditor(tex_frame, self, width=400, height=400)
        self.tex_editor.pack(fill=tk.BOTH, expand=True)

        region_frame = ttk.LabelFrame(left_frame, text="Regions", padding=5)
        region_frame.pack(fill=tk.BOTH, expand=False, pady=5)
        list_frame = ttk.Frame(region_frame)
        list_frame.pack(fill=tk.BOTH, expand=True)
        self.region_listbox = tk.Listbox(list_frame, height=5)
        self.region_listbox.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        scrollbar = ttk.Scrollbar(list_frame, orient=tk.VERTICAL, command=self.region_listbox.yview)
        scrollbar.pack(side=tk.RIGHT, fill=tk.Y)
        self.region_listbox.config(yscrollcommand=scrollbar.set)
        btn_frame = ttk.Frame(region_frame)
        btn_frame.pack(fill=tk.X, pady=2)
        ttk.Button(btn_frame, text="Edit", command=self.edit_selected_region).pack(side=tk.LEFT, padx=2)
        ttk.Button(btn_frame, text="Delete", command=self.delete_selected_region).pack(side=tk.LEFT)
        ttk.Button(btn_frame, text="Clear All", command=self.clear_all_regions).pack(side=tk.LEFT)
        
        var_frame = ttk.LabelFrame(left_frame, text="Custom Variables", padding=5)
        var_frame.pack(fill=tk.BOTH, expand=False, pady=5)
        list_frame_v = ttk.Frame(var_frame)
        list_frame_v.pack(fill=tk.BOTH, expand=True)
        self.var_listbox = tk.Listbox(list_frame_v, height=3)
        self.var_listbox.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        v_scroll = ttk.Scrollbar(list_frame_v, orient=tk.VERTICAL, command=self.var_listbox.yview)
        v_scroll.pack(side=tk.RIGHT, fill=tk.Y)
        self.var_listbox.config(yscrollcommand=v_scroll.set)
        var_btn_frame = ttk.Frame(var_frame)
        var_btn_frame.pack(fill=tk.X, pady=2)
        ttk.Button(var_btn_frame, text="Add", command=self.add_custom_var).pack(side=tk.LEFT, padx=2)
        ttk.Button(var_btn_frame, text="Edit", command=self.edit_custom_var).pack(side=tk.LEFT)
        ttk.Button(var_btn_frame, text="Delete", command=self.delete_custom_var).pack(side=tk.LEFT)

        right_frame = ttk.Frame(paned)
        paned.add(right_frame, weight=2)

        cube_frame = ttk.LabelFrame(right_frame, text="Cubes", padding=5)
        cube_frame.pack(fill=tk.BOTH, expand=True)
        self.cube_tree = ttk.Treeview(cube_frame, columns=("pos","size","pivot"), show="headings", height=8)
        self.cube_tree.heading("#0", text="Name")
        self.cube_tree.heading("pos", text="Pos")
        self.cube_tree.heading("size", text="Size")
        self.cube_tree.heading("pivot", text="Pivot")
        self.cube_tree.pack(fill=tk.BOTH, expand=True)
        cube_btn_frame = ttk.Frame(cube_frame)
        cube_btn_frame.pack(fill=tk.X)
        ttk.Button(cube_btn_frame, text="Add Cube", command=self.add_cube).pack(side=tk.LEFT, padx=2)
        ttk.Button(cube_btn_frame, text="Edit Cube", command=self.edit_cube).pack(side=tk.LEFT)
        ttk.Button(cube_btn_frame, text="Delete Cube", command=self.delete_cube).pack(side=tk.LEFT)

        preview_frame = ttk.LabelFrame(right_frame, text="3D Preview", padding=5)
        preview_frame.pack(fill=tk.BOTH, expand=True, pady=5)
        self.preview = GLPreview(preview_frame, self, width=400, height=300)
        self.preview.pack(fill=tk.BOTH, expand=True)
        
    def update_var_list(self):
        self.var_listbox.delete(0, tk.END)
        for v in self.model.custom_vars:
            self.var_listbox.insert(tk.END, f"{v.name} = {v.initial}")
            
    def add_custom_var(self):
        dialog = tk.Toplevel(self.root)
        dialog.title("Add Custom Variable")
        ttk.Label(dialog, text="Name:").grid(row=0, column=0, padx=5, pady=5)
        name_var = tk.StringVar()
        ttk.Entry(dialog, textvariable=name_var).grid(row=0, column=1)
        ttk.Label(dialog, text="Initial value:").grid(row=1, column=0, padx=5, pady=5)
        init_var = tk.StringVar(value="0.0f")
        ttk.Entry(dialog, textvariable=init_var).grid(row=1, column=1)
        def on_ok():
            name = name_var.get().strip()
            if not name:
                messagebox.showerror("Error", "Name required")
                return
            if any(v.name == name for v in self.model.custom_vars):
                messagebox.showerror("Error", "Variable already exists")
                return
            self.model.custom_vars.append(CustomVariable(name, init_var.get()))
            self.update_var_list()
            dialog.destroy()
        ttk.Button(dialog, text="OK", command=on_ok).grid(row=2, columnspan=2, pady=10)
        
    def edit_custom_var(self):
        sel = self.var_listbox.curselection()
        if not sel: return
        var = self.model.custom_vars[sel[0]]
        # аналогичный диалог с предзаполнением

    def delete_custom_var(self):
        sel = self.var_listbox.curselection()
        if not sel: return
        del self.model.custom_vars[sel[0]]
        self.update_var_list()

    # ---------- Texture handling ----------
    def load_texture(self):
        path = filedialog.askopenfilename(filetypes=[("PNG images", "*.png")])
        if not path: return
        self.model.texture_path = path
        self.texture_img = Image.open(path).convert("RGBA")
        self.tex_editor.set_image(self.texture_img)
        self.clear_all_regions()
        self.update_preview()

    def update_region_list(self):
        self.region_listbox.delete(0, tk.END)
        for r in self.model.regions:
            self.region_listbox.insert(tk.END, f"{r.name} ({r.w}x{r.h})")

    def edit_selected_region(self):
        sel = self.region_listbox.curselection()
        if not sel: return
        region = self.model.regions[sel[0]]
        self.tex_editor.edit_region(region)
        self.update_preview()

    def delete_selected_region(self):
        sel = self.region_listbox.curselection()
        if not sel: return
        region = self.model.regions[sel[0]]
        for cube in self.model.cubes:
            if cube.region == region.name:
                cube.region = None
        del self.model.regions[sel[0]]
        self.tex_editor.delete_region(region.name)
        self.update_region_list()
        self.update_preview()

    def clear_all_regions(self):
        self.model.regions.clear()
        self.tex_editor.clear_regions()
        self.update_region_list()
        self.update_preview()

    def update_preview(self):
        self.preview.refresh()

    # ---------- Cube management ----------
    def add_cube(self):
        self._cube_dialog(None)

    def edit_cube(self):
        selected = self.cube_tree.selection()
        if not selected: return
        item = selected[0]
        name = self.cube_tree.item(item, "text")
        cube = next((c for c in self.model.cubes if c.name == name), None)
        if cube:
            self._cube_dialog(cube)

    def _cube_dialog(self, cube):
        is_new = cube is None
        if is_new:
            cube = CubeDef(f"cube_{len(self.model.cubes)+1}")
        dialog = tk.Toplevel(self.root)
        dialog.title("Add Cube" if is_new else f"Edit Cube: {cube.name}")

        notebook = ttk.Notebook(dialog)
        notebook.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)

        # Вкладка General
        gen_frame = ttk.Frame(notebook)
        notebook.add(gen_frame, text="General")

        row = 0
        ttk.Label(gen_frame, text="Name:").grid(row=row, column=0, sticky="e", padx=5, pady=2)
        name_var = tk.StringVar(value=cube.name)
        ttk.Entry(gen_frame, textvariable=name_var).grid(row=row, column=1, padx=5, pady=2)
        row += 1

        ttk.Label(gen_frame, text="Pos (x y z):").grid(row=row, column=0, sticky="e")
        pos_var = tk.StringVar(value=f"{cube.pos[0]} {cube.pos[1]} {cube.pos[2]}")
        ttk.Entry(gen_frame, textvariable=pos_var).grid(row=row, column=1)
        row += 1

        ttk.Label(gen_frame, text="Size (w h d):").grid(row=row, column=0, sticky="e")
        size_var = tk.StringVar(value=f"{cube.size[0]} {cube.size[1]} {cube.size[2]}")
        ttk.Entry(gen_frame, textvariable=size_var).grid(row=row, column=1)
        row += 1

        ttk.Label(gen_frame, text="Pivot (px py pz):").grid(row=row, column=0, sticky="e")
        pivot_var = tk.StringVar(value=f"{cube.pivot[0]} {cube.pivot[1]} {cube.pivot[2]}")
        ttk.Entry(gen_frame, textvariable=pivot_var).grid(row=row, column=1)
        row += 1

        ttk.Label(gen_frame, text="Mirror:").grid(row=row, column=0, sticky="e")
        mirror_var = tk.BooleanVar(value=cube.mirror)
        ttk.Checkbutton(gen_frame, variable=mirror_var).grid(row=row, column=1, sticky="w")
        row += 1

        ttk.Label(gen_frame, text="Texture Region:").grid(row=row, column=0, sticky="e")
        region_var = tk.StringVar(value=cube.region if cube.region else "None")
        regions = ["None"] + [r.name for r in self.model.regions]
        combo = ttk.Combobox(gen_frame, textvariable=region_var, values=regions, state="readonly")
        combo.grid(row=row, column=1, sticky="w")
        row += 1

        # Вкладка Animation
        anim_frame = ttk.Frame(notebook)
        notebook.add(anim_frame, text="Animation")

        ttk.Label(anim_frame, text="Pitch (X rotation) formula:").grid(row=0, column=0, sticky="w", padx=5, pady=2)
        pitch_var = tk.StringVar(value=cube.anim_pitch)
        pitch_entry = ttk.Entry(anim_frame, textvariable=pitch_var, width=50)
        pitch_entry.grid(row=1, column=0, padx=5, pady=2, sticky="ew")
        ttk.Label(anim_frame, text="Yaw (Y rotation) formula:").grid(row=2, column=0, sticky="w", padx=5, pady=2)
        yaw_var = tk.StringVar(value=cube.anim_yaw)
        yaw_entry = ttk.Entry(anim_frame, textvariable=yaw_var, width=50)
        yaw_entry.grid(row=3, column=0, padx=5, pady=2, sticky="ew")
        ttk.Label(anim_frame, text="Roll (Z rotation) formula:").grid(row=4, column=0, sticky="w", padx=5, pady=2)
        roll_var = tk.StringVar(value=cube.anim_roll)
        roll_entry = ttk.Entry(anim_frame, textvariable=roll_var, width=50)
        roll_entry.grid(row=5, column=0, padx=5, pady=2, sticky="ew")

        # Подсказки и кнопки для вставки шаблонов
        help_frame = ttk.Frame(anim_frame)
        help_frame.grid(row=6, column=0, pady=10, sticky="w")
        ttk.Label(help_frame, text="Variables: time, speed, tick, headYRot, headXRot").pack(anchor="w")
        btn_frame = ttk.Frame(anim_frame)
        btn_frame.grid(row=7, column=0, pady=5, sticky="w")
        ttk.Button(btn_frame, text="cos(...)", command=lambda: self._insert_template(pitch_entry, "cos()")).pack(side=tk.LEFT, padx=2)
        ttk.Button(btn_frame, text="sin(...)", command=lambda: self._insert_template(pitch_entry, "sin()")).pack(side=tk.LEFT, padx=2)
        ttk.Button(btn_frame, text="M_PI", command=lambda: self._insert_template(pitch_entry, "M_PI")).pack(side=tk.LEFT, padx=2)
        ttk.Button(btn_frame, text="time", command=lambda: self._insert_template(pitch_entry, "time")).pack(side=tk.LEFT, padx=2)
        ttk.Button(btn_frame, text="speed", command=lambda: self._insert_template(pitch_entry, "speed")).pack(side=tk.LEFT, padx=2)
        ttk.Button(btn_frame, text="tick", command=lambda: self._insert_template(pitch_entry, "tick")).pack(side=tk.LEFT, padx=2)

        def on_save():
            try:
                pos = [float(v) for v in pos_var.get().split()]
                size = [int(v) for v in size_var.get().split()]
                pivot = [float(v) for v in pivot_var.get().split()]
                if len(pos)!=3 or len(size)!=3 or len(pivot)!=3:
                    raise ValueError
            except:
                messagebox.showerror("Error", "Invalid numbers")
                return
            new_name = name_var.get().strip()
            if not new_name:
                messagebox.showerror("Error", "Name required")
                return
            if is_new or new_name != cube.name:
                if any(c.name == new_name for c in self.model.cubes):
                    messagebox.showerror("Error", "Cube name must be unique")
                    return
            cube.name = new_name
            cube.pos = pos
            cube.size = size
            cube.pivot = pivot
            cube.mirror = mirror_var.get()
            reg = region_var.get()
            cube.region = reg if reg != "None" else None
            cube.anim_pitch = pitch_var.get().strip()
            cube.anim_yaw = yaw_var.get().strip()
            cube.anim_roll = roll_var.get().strip()

            if is_new:
                self.model.cubes.append(cube)
                self.cube_tree.insert("", "end", text=cube.name,
                                      values=(f"{pos[0]} {pos[1]} {pos[2]}",
                                              f"{size[0]} {size[1]} {size[2]}",
                                              f"{pivot[0]} {pivot[1]} {pivot[2]}"))
            else:
                item = self.cube_tree.selection()[0]
                self.cube_tree.item(item, text=cube.name,
                                    values=(f"{pos[0]} {pos[1]} {pos[2]}",
                                            f"{size[0]} {size[1]} {size[2]}",
                                            f"{pivot[0]} {pivot[1]} {pivot[2]}"))
            dialog.destroy()
            self.update_preview()

        ttk.Button(dialog, text="Save", command=on_save).pack(pady=5)

    def _insert_template(self, entry, text):
        # Вспомогательный метод для вставки шаблона в активное поле ввода
        entry.insert(tk.INSERT, text)

    def delete_cube(self):
        selected = self.cube_tree.selection()
        if not selected: return
        item = selected[0]
        name = self.cube_tree.item(item, "text")
        self.model.cubes = [c for c in self.model.cubes if c.name != name]
        self.cube_tree.delete(item)
        self.update_preview()

    # ---------- File I/O ----------
    def save_model(self):
        if not self.model.texture_path:
            messagebox.showerror("Error", "Load a texture first")
            return
        path = filedialog.asksaveasfilename(defaultextension=".json", filetypes=[("JSON", "*.json")])
        if path:
            self.model.name = self.model_name_var.get()
            self.model.save(path)

    def load_model(self):
        path = filedialog.askopenfilename(filetypes=[("JSON", "*.json")])
        if not path: return
        self.model.load(path)
        self.model_name_var.set(self.model.name)
        if os.path.exists(self.model.texture_path):
            self.texture_img = Image.open(self.model.texture_path).convert("RGBA")
            self.tex_editor.set_image(self.texture_img)
        else:
            messagebox.showwarning("Warning", "Texture file not found")
        self.update_region_list()
        self.tex_editor.clear_regions()
        for r in self.model.regions:
            self.tex_editor.draw_region(r)
        self.cube_tree.delete(*self.cube_tree.get_children())
        for c in self.model.cubes:
            self.cube_tree.insert("", "end", text=c.name,
                                  values=(f"{c.pos[0]} {c.pos[1]} {c.pos[2]}",
                                          f"{c.size[0]} {c.size[1]} {c.size[2]}",
                                          f"{c.pivot[0]} {c.pivot[1]} {c.pivot[2]}"))
        self.update_preview()

    # ---------- Code generation (.hpp only) ----------
    def generate_hpp(self):
        if not self.model.cubes:
            messagebox.showerror("Error", "No cubes defined")
            return
        dir_path = filedialog.askdirectory(title="Select output directory")
        if not dir_path: return
        model_name = self.model.name or "CustomModel"
        class_name = model_name + "Model"

        def region_uv_offset(region_name):
            if not region_name: return (0,0)
            reg = next((r for r in self.model.regions if r.name == region_name), None)
            if not reg: return (0,0)
            return (reg.x, reg.y)

        hpp_lines = []
        hpp_lines.append(f"#pragma once")
        hpp_lines.append(f"#include \"model/ModelPart.hpp\"")
        hpp_lines.append(f"#include <cmath>")
        hpp_lines.append(f"")
        hpp_lines.append(f"class {class_name} {{")
        hpp_lines.append(f"public:")
        for cube in self.model.cubes:
            hpp_lines.append(f"    ModelPart* {cube.name};")
        hpp_lines.append(f"")
        hpp_lines.append(f"    {class_name}();")
        hpp_lines.append(f"    ~{class_name}();")
        hpp_lines.append(f"    ")
        custom_args = ", ".join(f"float {v.name}" for v in self.model.custom_vars)
        if custom_args:
            custom_args = ", " + custom_args
        hpp_lines.append(f"    void render(float time, float speed, float tick{custom_args}, float scale);")
        hpp_lines.append(f"}};")
        hpp_lines.append(f"")
        hpp_lines.append(f"// Implementation")
        hpp_lines.append(f"#include <iostream>")
        hpp_lines.append(f"")
        hpp_lines.append(f"{class_name}::{class_name}() {{")
        for cube in self.model.cubes:
            u, v = region_uv_offset(cube.region)
            hpp_lines.append(f"    {cube.name} = new ModelPart({u}, {v});")
            if cube.mirror:
                hpp_lines.append(f"    {cube.name}->mirror = true;")
            hpp_lines.append(f"    {cube.name}->addBox({cube.pos[0]}f, {cube.pos[1]}f, {cube.pos[2]}f, {cube.size[0]}, {cube.size[1]}, {cube.size[2]});")
            hpp_lines.append(f"    {cube.name}->setPosition({cube.pivot[0]}f, {cube.pivot[1]}f, {cube.pivot[2]}f);")
            hpp_lines.append(f"")
        hpp_lines.append(f"}}")
        hpp_lines.append(f"")
        hpp_lines.append(f"{class_name}::~{class_name}() {{")
        for cube in self.model.cubes:
            hpp_lines.append(f"    delete {cube.name};")
        hpp_lines.append(f"}}")
        hpp_lines.append(f"")
        hpp_lines.append(f"void {class_name}::render(float time, float speed, float tick{custom_args}, float scale) {{")
        # Анимационные формулы
        for cube in self.model.cubes:
            if cube.anim_pitch:
                hpp_lines.append(f"    this->{cube.name}->pitch = {cube.anim_pitch};")
            if cube.anim_yaw:
                hpp_lines.append(f"    this->{cube.name}->yaw = {cube.anim_yaw};")
            if cube.anim_roll:
                hpp_lines.append(f"    this->{cube.name}->roll = {cube.anim_roll};")
        if any(c.anim_pitch or c.anim_yaw or c.anim_roll for c in self.model.cubes):
            hpp_lines.append(f"")
        for cube in self.model.cubes:
            hpp_lines.append(f"    this->{cube.name}->render(scale);")
        hpp_lines.append(f"}}")

        hpp_path = os.path.join(dir_path, f"{class_name}.hpp")
        with open(hpp_path, "w") as f:
            f.write("\n".join(hpp_lines))
        messagebox.showinfo("Success", f"Generated:\n{hpp_path}")

if __name__ == "__main__":
    root = tk.Tk()
    app = ModelEditorApp(root)
    root.geometry("1200x750")
    root.mainloop()