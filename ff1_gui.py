#!/usr/bin/env python3
"""
FatalFrame1 Save Converter - GUI (tkinter, Python 3 standard library only).
Runs on Windows, macOS and Linux. The conversion itself is in ff1_convert.py.

  python ff1_gui.py [--en | --zh]
"""
import json, locale, os, queue, sys, threading
import tkinter as tk
from tkinter import filedialog, font as tkfont, ttk

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import ff1_convert as ff

TITLE = 'FatalFrame1 Save Converter %s  -  零 ~zero~ / Fatal Frame (Xbox) JP ⇄ US' % ff.VERSION

STR = {
    'zh': {
        'l_src': '源存档文件夹', 'l_hd': '源主机 HD Key（可选）', 'l_hd2': '目标主机 HD Key', 'l_to': '转换为',
        'us': '美版 (US, 文件 G)', 'jp': '日版 (JP, 文件 N)', 'l_out': '输出文件夹', 'browse': '浏览…',
        'check': '检查', 'convert': '转换', 'lang': 'English',
        'clear': '清空预加载的模型、动画、音效（转换后读档卡死时再勾选）',
        'l_slot': '存档编号', 'keep': '不变',
        'multislot': '指定存档编号时只能选一个存档文件夹（找到了 %d 个）。\n',
        'intro': '选一个存档文件夹（里面有 G 或 N），或者整个 54430004 文件夹。\n'
                 '原文件不会被修改，结果写到 输出文件夹/54430004/<新文件夹名>。\n'
                 '欧版 (PAL) 存档不支持。\n'
                 '源 HD Key 只用来校验；只填一个 Key 时，源和目标共用。\n'
                 '「存档编号」可以把存档移到另一格（1–8），例如 1 号存成 8 号。\n'
                 '转换后读档卡 loading 的话，勾选「清空预加载…」再转一次。\n\n',
        'pick_src': '选择源存档文件夹', 'pick_out': '选择输出文件夹',
        'nosaves': '没有找到存档（需要有 G 或 N 文件的文件夹）。\n',
        'badkey': '源 HD Key 格式不对，需要 32 位十六进制。\n', 'badkey2': '目标 HD Key 格式不对。\n',
        'nosrc': '请先选源存档文件夹。\n', 'noout': '请选输出文件夹。\n', 'notarget': '请填目标主机的 HD Key。\n',
        'loadfail': '  读取失败：%s\n',
        'found': '找到 %d 个存档。\n',
        'item': '%s\n  %s 版，名称「%s」，开头校验和 %s\n',
        'sigok': '  签名 %d/%d ✓\n',
        'sigbad': '  签名 %d/%d ✗：源 HD Key 不对，或者不是日版/美版存档。\n',
        'signokey': '  （未填源 HD Key，不检查签名）\n',
        'ckbad': '  开头校验和不对，存档已损坏。\n',
        'noslot': '  读不出存档编号（SaveMeta.xbx）。\n',
        'unmapped': '  第 %d 项资源的文件编号 0x%X 在目标版本没有对应文件，未转换。\n',
        'exists': '  输出文件夹已存在且不为空，未覆盖：%s\n',
        'fail': '  失败：%s\n',
        'done1': '  → %s 版「%s」，%d 个文件编号已改，已重签\n     %s\n',
        'same': '  （源和目标是同一版本，只重签）\n',
        'cleared': '  （已清空预加载的模型、动画、音效）\n',
        'summary': '完成：%d 个成功，%d 个失败。把输出里的 54430004 文件夹整个放到目标主机。\n\n',
    },
    'en': {
        'l_src': 'Source save folder', 'l_hd': 'Source HD key (optional)', 'l_hd2': 'Target HD key', 'l_to': 'Convert to',
        'us': 'US (file G)', 'jp': 'JP (file N)', 'l_out': 'Output folder', 'browse': 'Browse…',
        'check': 'Check', 'convert': 'Convert', 'lang': '中文',
        'clear': 'Clear preloaded models/anims/sounds (only if the save hangs on load)',
        'l_slot': 'Slot', 'keep': 'same',
        'multislot': 'A slot number can only be set for a single save folder (found %d).\n',
        'intro': 'Pick one save folder (holding G or N), or the whole 54430004 folder.\n'
                 'Originals are not changed; results go to <output>/54430004/<new folder name>.\n'
                 'PAL saves are not supported.\n'
                 'Source HD key only verifies the save; with just one key, it is used for both.\n'
                 '"Slot" moves the save to another slot (1-8), e.g. save No.1 as No.8.\n'
                 'If a converted save hangs on load, tick "Clear preloaded..." and convert again.\n\n',
        'pick_src': 'Choose the source save folder', 'pick_out': 'Choose the output folder',
        'nosaves': 'No saves found (need folders that contain a G or N file).\n',
        'badkey': 'Source HD key must be 32 hex digits.\n', 'badkey2': 'Target HD key is not valid.\n',
        'nosrc': 'Choose the source save folder first.\n', 'noout': 'Choose an output folder.\n',
        'notarget': 'Enter the target HD key.\n',
        'loadfail': '  Cannot read: %s\n',
        'found': 'Found %d save(s).\n',
        'item': '%s\n  %s, name "%s", header checksum %s\n',
        'sigok': '  signature %d/%d ✓\n',
        'sigbad': '  signature %d/%d ✗: wrong source HD key, or not a JP/US save.\n',
        'signokey': '  (no source HD key, signature not checked)\n',
        'ckbad': '  Header checksum is wrong; the save is damaged.\n',
        'noslot': '  Cannot read the slot number (SaveMeta.xbx).\n',
        'unmapped': '  Resource %d: file id 0x%X has no counterpart in the target version; not converted.\n',
        'exists': '  Output folder exists and is not empty, not overwritten: %s\n',
        'fail': '  Failed: %s\n',
        'done1': '  → %s "%s", %d file id(s) remapped, re-signed\n     %s\n',
        'same': '  (same version as source: re-sign only)\n',
        'cleared': '  (preloaded models, animations and sounds cleared)\n',
        'summary': 'Done: %d converted, %d failed. Copy the whole 54430004 folder from the output to the target console.\n\n',
    },
}


def default_lang():
    if '--en' in sys.argv: return 'en'
    if '--zh' in sys.argv: return 'zh'
    try:
        if sys.platform == 'win32':
            import ctypes
            return 'zh' if ctypes.windll.kernel32.GetUserDefaultUILanguage() & 0x3FF == 0x04 else 'en'
    except Exception:
        pass
    loc = os.environ.get('LC_ALL') or os.environ.get('LC_MESSAGES') or os.environ.get('LANG') or ''
    if not loc:
        try: loc = locale.getlocale()[0] or ''
        except Exception: loc = ''
    return 'zh' if loc.lower().startswith(('zh', 'chinese')) else 'en'


def save_file(folder):
    """'G', 'N' or None: which save file the folder holds."""
    try: names = {f.upper() for f in os.listdir(folder)}
    except OSError: return None
    return 'G' if 'G' in names else 'N' if 'N' in names else None


def unmapped(save, to, clear):
    """First (index, file id) in the resource table with no counterpart in `to`, or None."""
    if save.region == to: return None
    m = ff.IDMAP[(save.region, to)]
    for i, fid, typ in save.resources():
        if clear and typ in ff.RES_DROPPED: continue
        if fid >= ff.NFILES[save.region] or fid not in m: return i, fid
    return None


CONFIG = os.path.join(os.path.expanduser('~'), '.ff1_converter.json')


def load_config():
    try:
        with open(CONFIG, encoding='utf-8') as f: return json.load(f)
    except (OSError, ValueError): return {}


def save_config(cfg):
    try:
        with open(CONFIG, 'w', encoding='utf-8') as f: json.dump(cfg, f)
    except OSError: pass


class App:
    def __init__(self, root):
        self.root, self.lang = root, default_lang()
        self.q = queue.Queue()
        self.busy = False
        root.title(TITLE)
        root.minsize(700, 480)
        root.geometry('860x660')

        f = ttk.Frame(root, padding=12)
        f.pack(fill='both', expand=True)
        f.columnconfigure(1, weight=1)
        f.rowconfigure(8, weight=1)
        self.w = {}
        self.src, self.hd, self.hd2, self.out = (tk.StringVar() for _ in range(4))
        self.cfg = load_config()
        self.to, self.clear = tk.StringVar(value='jp' if self.cfg.get('to') == 'jp' else 'us'), tk.BooleanVar()
        self.to.trace_add('write', lambda *_: self.remember_to())
        self.slot = tk.StringVar()
        pad = dict(padx=4, pady=4)

        def label(key, row):
            self.w[key] = ttk.Label(f); self.w[key].grid(row=row, column=0, sticky='w', **pad)

        label('l_src', 0)
        ttk.Entry(f, textvariable=self.src).grid(row=0, column=1, sticky='ew', **pad)
        self.w['br_src'] = ttk.Button(f, command=self.pick_src); self.w['br_src'].grid(row=0, column=2, **pad)
        label('l_hd', 1)
        ttk.Entry(f, textvariable=self.hd, font='TkFixedFont').grid(row=1, column=1, sticky='ew', **pad)
        label('l_hd2', 2)
        ttk.Entry(f, textvariable=self.hd2, font='TkFixedFont').grid(row=2, column=1, sticky='ew', **pad)
        label('l_to', 3)
        rb = ttk.Frame(f); rb.grid(row=3, column=1, sticky='w', **pad)
        self.w['us'] = ttk.Radiobutton(rb, variable=self.to, value='us'); self.w['us'].pack(side='left', padx=(0, 24))
        self.w['jp'] = ttk.Radiobutton(rb, variable=self.to, value='jp'); self.w['jp'].pack(side='left', padx=(0, 32))
        self.w['l_slot'] = ttk.Label(rb); self.w['l_slot'].pack(side='left', padx=(0, 6))
        self.w['slot'] = ttk.Combobox(rb, textvariable=self.slot, state='readonly', width=6)
        self.w['slot'].pack(side='left')
        self.w['clear'] = ttk.Checkbutton(f, variable=self.clear)
        self.w['clear'].grid(row=4, column=1, columnspan=2, sticky='w', **pad)
        label('l_out', 5)
        ttk.Entry(f, textvariable=self.out).grid(row=5, column=1, sticky='ew', **pad)
        self.w['br_out'] = ttk.Button(f, command=self.pick_out); self.w['br_out'].grid(row=5, column=2, **pad)

        bar = ttk.Frame(f); bar.grid(row=6, column=1, columnspan=2, sticky='ew', pady=(8, 8))
        self.w['check'] = ttk.Button(bar, command=lambda: self.start(False)); self.w['check'].pack(side='left', padx=4)
        self.w['convert'] = ttk.Button(bar, command=lambda: self.start(True)); self.w['convert'].pack(side='left', padx=4)
        self.w['lang'] = ttk.Button(bar, command=self.toggle_lang); self.w['lang'].pack(side='right', padx=4)

        lf = ttk.Frame(f); lf.grid(row=8, column=0, columnspan=3, sticky='nsew', **pad)
        lf.rowconfigure(0, weight=1); lf.columnconfigure(0, weight=1)
        self.log_w = tk.Text(lf, wrap='word', state='disabled', height=12, font='TkDefaultFont', relief='sunken', borderwidth=1)
        sb = ttk.Scrollbar(lf, command=self.log_w.yview)
        self.log_w.configure(yscrollcommand=sb.set)
        self.log_w.grid(row=0, column=0, sticky='nsew'); sb.grid(row=0, column=1, sticky='ns')

        self.apply_lang()
        self.show_intro()
        root.after(50, self.pump)

    # ---- text
    def S(self, k): return STR[self.lang][k]

    def apply_lang(self):
        for k in ('l_src', 'l_hd', 'l_hd2', 'l_to', 'us', 'jp', 'l_slot', 'clear', 'l_out', 'check', 'convert', 'lang'):
            self.w[k].configure(text=self.S(k))
        cur = self.slot.get()
        self.w['slot'].configure(values=[self.S('keep')] + [str(i) for i in range(1, 9)])
        self.slot.set(cur if cur.isdigit() else self.S('keep'))
        self.w['br_src'].configure(text=self.S('browse')); self.w['br_out'].configure(text=self.S('browse'))

    def toggle_lang(self):
        self.lang = 'en' if self.lang == 'zh' else 'zh'
        self.apply_lang()
        if not self.log_used: self.show_intro()

    def show_intro(self):
        self.log_w.configure(state='normal')
        self.log_w.delete('1.0', 'end')
        self.log(self.S('intro'))
        self.log_used = False   # intro only: re-shown in the new language on toggle

    def log(self, s):
        self.log_used = True
        self.log_w.configure(state='normal')
        self.log_w.insert('end', s)
        self.log_w.see('end')
        self.log_w.configure(state='disabled')

    def pump(self):
        try:
            while True:
                item = self.q.get_nowait()
                if item is None: self.set_busy(False)
                else: self.log(item)
        except queue.Empty:
            pass
        self.root.after(50, self.pump)

    # ---- folders
    def pick_src(self):
        p = filedialog.askdirectory(title=self.S('pick_src'), initialdir=self.src.get() or None, mustexist=True)
        if not p: return
        p = os.path.normpath(p)
        self.src.set(p)
        self.default_out(p)

    def remember_to(self):
        self.cfg['to'] = self.to.get()
        save_config(self.cfg)

    def pick_out(self):
        p = filedialog.askdirectory(title=self.S('pick_out'), initialdir=self.out.get() or None)
        if p: self.out.set(os.path.normpath(p))

    def default_out(self, src):
        b = src.rstrip('\\/ ')
        if save_file(b): b = os.path.dirname(b)
        if os.path.basename(b).upper() == ff.TITLE_ID: b = os.path.dirname(b)
        self.out.set(os.path.join(b, 'FF1_converted'))

    # ---- work
    def set_busy(self, busy):
        self.busy = busy
        for k in ('check', 'convert', 'br_src', 'br_out'):
            self.w[k].configure(state='disabled' if busy else 'normal')
        self.root.configure(cursor='watch' if busy else '')

    def start(self, convert):
        if self.busy: return
        src = self.src.get().strip().rstrip('\\/')
        if not src: return self.log(self.S('nosrc'))
        try: k1 = ff.parse_hdkey(self.hd.get()) if self.hd.get().strip() else None
        except ValueError: return self.log(self.S('badkey'))
        try: k2 = ff.parse_hdkey(self.hd2.get()) if self.hd2.get().strip() else k1
        except ValueError: return self.log(self.S('badkey2'))
        if convert and not k2: return self.log(self.S('notarget'))
        out = self.out.get().strip().rstrip('\\/')
        if convert and not out: return self.log(self.S('noout'))
        try: folders = ff.find_saves([src])
        except (ValueError, OSError): return self.log(self.S('nosaves'))
        slot = int(self.slot.get()) if self.slot.get().isdigit() else None
        if convert and slot and len(folders) > 1: return self.log(self.S('multislot') % len(folders))
        self.set_busy(True)
        args = (folders, convert, k1, k2, out, self.to.get(), slot, self.clear.get(), dict(STR[self.lang]))
        threading.Thread(target=self.work, args=args, daemon=True).start()

    def work(self, folders, convert, k1, k2, out, to, new_slot, clear, S):
        log, ok, bad = self.q.put, 0, 0
        try:
            log(S['found'] % len(folders))
            for folder in folders:
                try: s = ff.Save(folder)
                except (ValueError, OSError) as e:
                    log('%s\n' % folder); log(S['loadfail'] % e); bad += 1; continue
                ck = s.checksum_ok()
                log(S['item'] % (folder, ff.REGIONS[s.region]['label'], s.name or '?', '✓' if ck else '✗'))
                sig = None
                if k1:
                    sig = s.sig_ok(k1)
                    log(S['sigok' if sig == ff.NBLOCK + 1 else 'sigbad'] % (sig, ff.NBLOCK + 1))
                else:
                    log(S['signokey'])
                if not convert: continue
                if sig is not None and sig != ff.NBLOCK + 1: bad += 1; continue
                if not ck: log(S['ckbad']); bad += 1; continue
                slot = new_slot or s.slot()
                if not slot: log(S['noslot']); bad += 1; continue
                if s.region == to: log(S['same'])
                um = unmapped(s, to, clear)
                if um: log(S['unmapped'] % um); bad += 1; continue
                name = ff.REGIONS[to]['name'] % slot
                od = os.path.join(out, ff.TITLE_ID, ff.folder_name(name))
                if os.path.isdir(od) and os.listdir(od): log(S['exists'] % od); bad += 1; continue
                msgs = []
                try:
                    # signature already checked above, so no source key here
                    od = ff.convert(s, to, None, k2, out, slot, log=msgs.append, clear_res=clear)
                except (ValueError, OSError) as e:
                    log(S['fail'] % e); bad += 1; continue
                log(S['done1'] % (ff.REGIONS[to]['label'], name, self.count_remapped(s, to, clear), od))
                if clear and s.region != to: log(S['cleared'])
                ok += 1
            log(S['summary'] % (ok, bad) if convert else '\n')
        except Exception as e:  # never leave the window stuck in busy state
            log(S['fail'] % e)
        finally:
            self.q.put(None)

    @staticmethod
    def count_remapped(s, to, clear):
        if s.region == to: return 0
        m = ff.IDMAP[(s.region, to)]
        return sum(1 for i, fid, typ in s.resources() if not (clear and typ in ff.RES_DROPPED) and m[fid] != fid)


def main():
    if sys.platform == 'win32':
        try:
            import ctypes
            ctypes.windll.shcore.SetProcessDpiAwareness(1)
        except Exception:
            pass
    root = tk.Tk()
    if sys.platform == 'win32':
        # Segoe UI has no CJK glyphs, so mixed zh/en text falls back per character; YaHei covers both
        fam = next((f for f in ('Microsoft YaHei UI', 'Microsoft YaHei') if f in tkfont.families(root)), None)
        if fam:
            for n in ('TkDefaultFont', 'TkTextFont', 'TkMenuFont', 'TkHeadingFont', 'TkCaptionFont', 'TkTooltipFont'):
                tkfont.nametofont(n).configure(family=fam)
    try: ttk.Style(root).theme_use('vista' if sys.platform == 'win32' else 'clam' if sys.platform.startswith('linux') else 'aqua')
    except tk.TclError: pass
    App(root)
    root.mainloop()


if __name__ == '__main__':
    main()
