from pathlib import Path

from PIL import Image, ImageDraw, ImageFont


OUT = Path(__file__).with_name("easy-display-lvgl-initial-architecture.png")
W, H = 1800, 1280
FONT = r"C:\Windows\Fonts\msyh.ttc"
FONT_BOLD = r"C:\Windows\Fonts\msyhbd.ttc"


def font(size, bold=False):
    return ImageFont.truetype(FONT_BOLD if bold else FONT, size)


img = Image.new("RGB", (W, H), "#f8fafc")
d = ImageDraw.Draw(img)


def rounded(box, fill, outline="#2563eb", width=3, radius=18):
    d.rounded_rectangle(box, radius=radius, fill=fill, outline=outline, width=width)


def centered(box, text, size=28, color="#0f172a", bold=False, spacing=7):
    f = font(size, bold)
    bb = d.multiline_textbbox((0, 0), text, font=f, spacing=spacing, align="center")
    x = (box[0] + box[2] - (bb[2] - bb[0])) / 2
    y = (box[1] + box[3] - (bb[3] - bb[1])) / 2 - bb[1]
    d.multiline_text((x, y), text, font=f, fill=color, spacing=spacing, align="center")


def arrow(x1, y1, x2, y2, color="#0369a1", width=5):
    d.line((x1, y1, x2, y2), fill=color, width=width)
    if abs(x2 - x1) > abs(y2 - y1):
        s = 1 if x2 > x1 else -1
        pts = [(x2, y2), (x2 - 16 * s, y2 - 10), (x2 - 16 * s, y2 + 10)]
    else:
        s = 1 if y2 > y1 else -1
        pts = [(x2, y2), (x2 - 10, y2 - 16 * s), (x2 + 10, y2 - 16 * s)]
    d.polygon(pts, fill=color)


d.text((W / 2, 48), "LVGL Button 初始可运行架构", anchor="mm", font=font(50, True), fill="#0f172a")
d.text((W / 2, 95), "三仓库分层 · LVGL v9.5.0 固定版本 · 阻塞式软件 SPI（暂不使用 DMA）",
       anchor="mm", font=font(24), fill="#475569")

# Application repository
rounded((55, 135, 1745, 420), "#eff6ff", "#1d4ed8", 4, 24)
d.text((85, 165), "① 应用仓库  easy_display_lvgl_button_app", font=font(31, True), fill="#1d4ed8")

app = (105, 225, 475, 365)
lvgl = (555, 225, 970, 365)
cfg = (1050, 225, 1695, 365)
rounded(app, "#ffffff")
rounded(lvgl, "#dbeafe")
rounded(cfg, "#ffffff")
centered(app, "app/main.c + app/ui.c\n创建并响应 LVGL Button", 27, bold=True)
centered(lvgl, "official LVGL v9.5.0\nGit submodule: external/lvgl", 27, bold=True)
centered(cfg, "应用拥有策略与组装\nlv_conf.h · 10 行绘制缓冲区\n硬件选择 · 依赖版本", 25)
arrow(app[2], 295, lvgl[0], 295)
arrow(lvgl[2], 295, cfg[0], 295, "#64748b", 3)

# Port repository
rounded((55, 465, 1745, 745), "#ecfeff", "#0891b2", 4, 24)
d.text((85, 495), "② LVGL 适配层仓库  easy_lvgl_port", font=font(31, True), fill="#0e7490")
generic = (105, 555, 560, 690)
binding = (670, 555, 1125, 690)
contract = (1235, 555, 1695, 690)
rounded(generic, "#ffffff", "#0891b2")
rounded(binding, "#cffafe", "#0891b2")
rounded(contract, "#ffffff", "#0891b2")
centered(generic, "easy_lvgl_port\nLVGL display / indev 回调", 26, bold=True)
centered(binding, "easy_display_binding\n连接统一设备接口", 26, bold=True)
centered(contract, "RGB565 字节交换\n刷新完成通知 · 触摸状态映射", 25)
arrow(generic[2], 623, binding[0], 623)
arrow(binding[2], 623, contract[0], 623)
arrow(760, 420, 760, 555)

# Driver repository
rounded((55, 790, 1745, 1130), "#f0fdf4", "#16a34a", 4, 24)
d.text((85, 820), "③ 驱动与板级仓库  easy_display", font=font(31, True), fill="#15803d")
api = (105, 890, 455, 1055)
adapter = (535, 890, 905, 1055)
driver = (985, 890, 1355, 1055)
board = (1435, 890, 1695, 1055)
rounded(api, "#ffffff", "#16a34a")
rounded(adapter, "#dcfce7", "#16a34a")
rounded(driver, "#ffffff", "#16a34a")
rounded(board, "#dcfce7", "#16a34a")
centered(api, "统一公共接口\neasy_display\neasy_pointer", 25, bold=True)
centered(adapter, "设备适配器\nILI9341 display\nXPT2046 pointer", 24, bold=True)
centered(driver, "现有芯片驱动\nLCD_DRIVER\nTOUCH_CONTROLLER", 24)
centered(board, "STM32F407VET6\n阻塞式软件 SPI", 24, bold=True)
arrow(api[2], 972, adapter[0], 972)
arrow(adapter[2], 972, driver[0], 972)
arrow(driver[2], 972, board[0], 972)
arrow(885, 745, 885, 890)

# Physical devices
panel = (340, 1180, 740, 1250)
touch = (1060, 1180, 1460, 1250)
rounded(panel, "#fff7ed", "#ea580c", 3, 15)
rounded(touch, "#fff7ed", "#ea580c", 3, 15)
centered(panel, "ILI9341 TFT · 240 × 320 · RGB565", 23, bold=True)
centered(touch, "XPT2046 电阻触摸", 23, bold=True)
arrow(1525, 1055, 1525, 1160, "#ea580c", 4)
d.line((1525, 1160, 540, 1160, 540, 1180), fill="#ea580c", width=4)
d.line((1525, 1160, 1260, 1160, 1260, 1180), fill="#ea580c", width=4)
d.polygon([(540, 1180), (530, 1164), (550, 1164)], fill="#ea580c")
d.polygon([(1260, 1180), (1250, 1164), (1270, 1164)], fill="#ea580c")

img.save(OUT, optimize=True)
print(OUT)
