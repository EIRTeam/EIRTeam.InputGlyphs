import json
from io import StringIO
from pathlib import Path
from typing import Dict, List


class ThemeInfo:
    graphics: List[int]
    graphics_abxy_overrides: List[int]

    def __init__(self):
        self.graphics = []
        self.graphics_abxy_overrides = []


class InputTypeInfo:
    themes: List[ThemeInfo]

    def __init__(self):
        self.themes = []


def make_glyphs_header(target, source, env):
    json_f = json.load(open(str(source[0]), "r"))
    with open(str(target[0]), "w") as f:
        f.write("#ifndef DEFAULT_INPUT_GLYPHS_GEN_H\n")
        f.write("#define DEFAULT_INPUT_GLYPHS_GEN_H\n\n")

        f.write('#include "input_glyphs.h"\n')

        svg_files: List[str] = []
        svg_files_map: Dict[str, int] = {}

        input_type_infos: List[InputTypeInfo] = []

        for input_type in json_f["input_types"]:
            input_type_nfo = InputTypeInfo()
            input_type_infos.append(input_type_nfo)
            for theme in input_type["themes"]:
                theme_nfo = ThemeInfo()
                input_type_nfo.themes.append(theme_nfo)
                for graphic_path in theme["graphics"]:
                    if not graphic_path:
                        theme_nfo.graphics.append(-1)
                        continue
                    if graphic_path not in svg_files_map:
                        svg_files.append(graphic_path)
                        svg_files_map[graphic_path] = len(svg_files) - 1
                    theme_nfo.graphics.append(svg_files_map[graphic_path])
                for graphic_abxy_override_path in theme["graphics_abxy_overrides"]:
                    if not graphic_abxy_override_path:
                        theme_nfo.graphics_abxy_overrides.append(-1)
                        continue
                    if graphic_abxy_override_path not in svg_files_map:
                        svg_files.append(graphic_abxy_override_path)
                        svg_files_map[graphic_abxy_override_path] = len(svg_files) - 1
                    theme_nfo.graphics_abxy_overrides.append(svg_files_map[graphic_abxy_override_path])

        # Write SVG table

        f.write("static const char* __glyph_icons [] = {\n")
        for file_name in svg_files:
            f.write('\t"')
            with open(Path(source[0].srcnode().abspath).parent / file_name, "rb") as svgf:
                b = svgf.read(1)
                while len(b) == 1:
                    f.write("\\" + str(hex(ord(b)))[1:])
                    b = svgf.read(1)
            f.write('",\n')

        f.write("};\n\n")

        input_type_graphics_string = StringIO()
        input_type_graphics_abxy_override_string = StringIO()

        input_type_graphics_string.write(
            "static const int __glyph_icons_map[InputGlyphsConstants::INPUT_TYPE_MAX][InputGlyphStyle::GLYPH_STYLE_THEME_COUNT][InputGlyphsConstants::InputOrigin::INPUT_ORIGIN_COUNT] = {\n"
        )
        input_type_graphics_abxy_override_string.write(
            "static const int __glyph_icons_abxy_override_map[InputGlyphsConstants::INPUT_TYPE_MAX][InputGlyphStyle::GLYPH_STYLE_THEME_COUNT][3*4] = {\n"
        )

        for input_type in input_type_infos:
            # Input type
            input_type_graphics_string.write("\t{\n")
            input_type_graphics_abxy_override_string.write("\t{\n")
            for theme in input_type.themes:
                # Themes
                input_type_graphics_string.write("\t\t{")
                input_type_graphics_abxy_override_string.write("\t\t{")
                for graphic in theme.graphics:
                    input_type_graphics_string.write(f" {graphic},")
                for graphic_abxy in theme.graphics_abxy_overrides:
                    input_type_graphics_abxy_override_string.write(f" {graphic_abxy},")
                input_type_graphics_abxy_override_string.write(" },\n")
                input_type_graphics_string.write(" },\n")
            input_type_graphics_abxy_override_string.write("\t},\n")
            input_type_graphics_string.write("\t},\n")
        input_type_graphics_string.write("};\n")
        input_type_graphics_abxy_override_string.write("};\n")

        f.write(input_type_graphics_string.getvalue())
        f.write(input_type_graphics_abxy_override_string.getvalue())

        f.write("#endif\n")
