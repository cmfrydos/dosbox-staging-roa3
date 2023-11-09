import re


def generate_pretty_name(field_name):
    # Mapping of specific abbreviations to their full forms
    abbreviation_map = {
        "ap": "AP",  # Abenteuerpunkte
        "ae": "AE",  # Astralenergie
        "mr": "MR",  # Magieresistenz
        "rs": "RS",  # Rüstungsschutz
        "le": "LE",  # Lebensenergie
        "at": "AT",  # Attacke
        "pa": "PA",  # Parade
        "bp": "BP",  # Bewegungspunkte
        "atpa": "AT PA",
        "be": "BE",  # Behinderung

        "unkn": "Unknown"
        # Add more specific abbreviations as needed
    }

    # If field name consists of lowercase letters followed by numbers or
    # underscore, split and capitalize
    # Splitting by numbers and underscores
    parts = re.split('(\\d+|_+)', field_name)
    parts = [part for part in parts if part != "_" and part != ""]

    capitalized_parts = []

    for part in parts:
        # If a word is in the abbreviation map, replace it
        if part in abbreviation_map:
            capitalized_parts.append(abbreviation_map[part])
        else:
            capitalized_parts.append(
                part.upper() if len(part) == 1 else part.capitalize())

    return ' '.join(capitalized_parts)


names_attrib = [
    "MU",
    "KL",
    "CH",
    "FF",
    "GE",
    "IN",
    "KK",
    "AG",
    "HA",
    "RA",
    "GG",
    "TA",
    "NG",
    "JZ"]
names_skill = [
    "Waffenlos",
    "Hiebwaffen",
    "Stichwaffen",
    "Schwerter",
    "Äxte",
    "Speere",
    "Zweihänder",
    "Schusswaffen",
    "Wurfwaffen",
    "Akrobatik",
    "Klettern",
    "Körperbeh.",
    "Reiten",
    "Schleichen",
    "Schwimmen",
    "Selbstbeh.",
    "Tanzen",
    "Verstecken",
    "Zechen",
    "Bekehren",
    "Betören",
    "Feilschen",
    "Gassenwissen",
    "Lügen",
    "Menschenkenntnis",
    "Schätzen",
    "Fährtensuchen",
    "Fesseln",
    "Orientierung",
    "Pflanzenkunde",
    "Tierkunde",
    "Wildnisleben",
    "Alchimie",
    "Alte Sprachen",
    "Geographie",
    "Geschichte",
    "Götter/Kulte",
    "Kriegskunst",
    "Lesen",
    "Magiekunde",
    "Sprachen",
    "Abrichten",
    "Fahrzeuge",
    "Falschspiel",
    "Heilen Gift",
    "Heilen Krankheit",
    "Heilen Wunden",
    "Musizieren",
    "Schlösser",
    "Taschendieb",
    "Gefahrensinn",
    "Sinnenschärfe"]
names_spell = [
    "DUMMY",
    "Beherrschung brechen",
    "Destructibo",
    "Gardianum",
    "Illusionen zerstören",
    "Verwandlung beenden",
    "Band & Fessel",
    "Bannbaladin",
    "Böser Blick",
    "Große Gier",
    "Große Verwirrung",
    "Herr der Tiere",
    "Horriphobus",
    "Mag. Raub",
    "Respondami",
    "Sanftmut",
    "Somnigravis",
    "Zwingtanz",
    "Furor Blut",
    "Geister bannen",
    "Geister rufen",
    "Heptagon",
    "Krähenruf",
    "Skelettarius",
    "Elementar herbeirufen",
    "Nihilatio Gravitas",
    "Solidrid Farbenspiel",
    "Axxeleraus",
    "Foramen",
    "Motoricus",
    "Spurlos, Trittlos",
    "Transversalis",
    "Über Eis",
    "Balsam",
    "Hexenspeichel",
    "Klarum Purum",
    "Ruhe Körper",
    "Tiere heilen",
    "Adleraug",
    "Analüs",
    "Eigenschaften",
    "Exposami",
    "Odem Arcanum",
    "Penetrizzel",
    "Sensibar",
    "Chamaelioni",
    "Duplicatus",
    "Harmlos",
    "Hexenknoten",
    "Blitz",
    "Ecliptifactus",
    "Eisenrost",
    "Fulminictus",
    "Ignifaxius",
    "Plumbumbarum",
    "Radau",
    "Saft, Kraft, Monstermacht",
    "Scharfes Auge",
    "Hexenblick",
    "Nekropathia",
    "Adler, Wolf",
    "Arcano Psychostabilis",
    "Armatrutz",
    "CH steigern",
    "Feuerbann",
    "FF steigern",
    "GE steigern",
    "IN steigern",
    "KK steigern",
    "KL steigern",
    "MU steigern",
    "Mutabili",
    "Paralü",
    "Salander",
    "See & Fluss",
    "Visibili",
    "Abvenenum",
    "Aeolitus",
    "Brenne",
    "Claudibus",
    "Dunkelheit",
    "Erstarre",
    "Flim Flam",
    "Schmelze",
    "Silentium",
    "Sturmgebrüll"]


# Updating the generate_cpp_content function to use the
# generate_pretty_name function
def generate_cpp_content(header_content):
    pattern = re.compile(
        r'(?P<type>(?:signed\s*)?\w+)\s+(?P<name>\w+)(?:\s*\[\s*(?P<size1>\d+)\s*\])?(?:\s*\[\s*(?P<size2>\d+)\s*\])?\s*;')
    offset = 0
    dictionary_entries = []

    for match in pattern.finditer(header_content):
        field_type = match.group('type')
        field_name = match.group('name')
        pretty_name = generate_pretty_name(field_name)

        field_size1 = int(match.group('size1') or 1)
        field_size2 = int(match.group('size2') or 1)

        if field_name == "attribs":
            subfields = ["Normal", "Current", "Mod"]
            for i in range(field_size1):
                for j in range(field_size2):
                    pretty_name = f"{names_attrib[i]} {subfields[j]}"
                    comment = f"// {offset}, {pretty_name}, 1 Byte"
                    entry = f"{comment}\n{{0x{offset:02x}, {{1, u8\"{pretty_name}\"}}}},"
                    dictionary_entries.append(entry)
                    offset += 1
            continue

        if field_name == 'skills':
            for i, skill_name in enumerate(names_skill):
                entry = f"// {offset}, {skill_name}, 1 Byte\n{{0x{offset:02x}, {{1, u8\"{skill_name}\"}}}},"
                dictionary_entries.append(entry)
                offset += 1
            continue

        elif field_name == 'spells':
            for i, spell_name in enumerate(names_spell):
                entry = f"// {offset}, {spell_name}, 1 Byte\n{{0x{offset:02x}, {{1, u8\"{spell_name}\"}}}},"
                dictionary_entries.append(entry)
                offset += 1
            continue

        if field_type == "signed char":
            type_size = 1
        elif field_type == "signed short":
            type_size = 2
        elif field_type == "signed int":
            type_size = 4
        else:
            type_size = 1  # Default size

        total_field_size = type_size * field_size1 * field_size2

        comment = f"// {offset}, {pretty_name}, {total_field_size} Byte"
        entry = f"{comment}\n{{0x{offset:02x}, {{{total_field_size}, u8\"{pretty_name}\"}}}},"
        dictionary_entries.append(entry)
        offset += total_field_size

    cpp_content = "#include <map>\n"
    cpp_content += "#include <string>\n"
    cpp_content += "// Dictionary definition\n"
    cpp_content += "std::map<int, std::tuple<int, std::string>> hero_attr_map = {\n"
    cpp_content += '\n'.join(dictionary_entries)
    cpp_content += "\n};\n"

    return cpp_content


# Sample header_content (for testing)
header_content = """
struct struct_hero {
    char preamble [118];
    signed char w_at_mod;
    signed char w_pa_mod;
    signed char w_type;
    // ... (and so on)
};
"""

# Let's use the function with the sample header_content to see if the
# issue with arrays has been addressed
generate_cpp_content(header_content)


def generate_cpp_from_header(header_filename, cpp_filename):
    # Read the .h file
    with open(header_filename, 'r') as header_file:
        header_content = header_file.read()

    # Generate the .cpp content using the function we defined earlier
    cpp_content = generate_cpp_content(header_content)

    # Write the generated content to the .cpp file
    with open(cpp_filename, 'w') as cpp_file:
        cpp_file.write(cpp_content)

# For demonstration purposes, let's write the sample header_content to a .h file,
# then read it and generate the corresponding .cpp file:


header_filename = './hero.h'
cpp_filename = './hero_annotated.cpp'

generate_cpp_from_header(header_filename, cpp_filename)

# Returning the path to the generated .cpp file for verification
print(cpp_filename)


res = generate_cpp_content(header_content)
print(res)
