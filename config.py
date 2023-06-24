def can_build(env, platform):
    env.module_add_dependencies("input_glyphs", ["svg"])
    return True


def configure(env):
    pass


def get_doc_path():
    return "doc_classes"


def get_doc_classes():
    return ["HBInputGlyphsSingleton", "HBInputGlyphIcon", "HBInputActionGlyph"]
