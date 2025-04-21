import pytest
from mechanism_configuration import Parser, ReactionType


def validate_species(species):
    # Define the expected species and their required attributes
    expected_species = {
        "A": {},
        "B": {"tracer_type": "AEROSOL"},
        "C": {"tracer_type": "THIRD_BODY"},
        "M": {},
        "H2O2": {
            "henrys_law_constant_298": 1.011596348,
            "henrys_law_constant_exponential_factor": 6340,
            "diffusion_coefficient": 1.46e-05,
            "n_star": 1.74,
            "molecular_weight": 0.0340147,
            "density": 1000.0,
            "unknown_properties": { "__absolute tolerance": "1.0e-10" },
        },
        "ethanol": {
            "diffusion_coefficient": 0.95e-05,
            "n_star": 2.55,
            "molecular_weight": 0.04607,
            "unknown_properties": { "__absolute tolerance": "1.0e-20" },
        },
        "ethanol_aq": {
            "molecular_weight": 0.04607,
            "density": 1000.0,
            "unknown_properties": { "__absolute tolerance": "1.0e-20" },
        },
        "H2O2_aq": {
            "molecular_weight": 0.0340147,
            "density": 1000.0,
            "unknown_properties": { "__absolute tolerance": "1.0e-10" },
        },
        "H2O_aq": {
            "density": 1000.0,
            "molecular_weight": 0.01801,
        },
        "aerosol stuff": {
            "molecular_weight": 0.5,
            "density": 1000.0,
            "unknown_properties": { "__absolute tolerance": "1.0e-20" },
        },
        "more aerosol stuff": {
            "molecular_weight": 0.2,
            "density": 1000.0,
            "unknown_properties": { "__absolute tolerance": "1.0e-20" },
        },
    }

    # Create a dictionary for quick lookup of species by name
    species_dict = {sp.name: sp for sp in species}

    # Validate each expected species
    for name, attributes in expected_species.items():
        assert name in species_dict, f"Species '{name}' is missing."
        for attr, expected_value in attributes.items():
            assert (
                hasattr(species_dict[name], attr)
            ), f"Attribute '{attr}' is missing for species '{name}'."
            assert getattr(species_dict[name], attr) == expected_value, (
                f"Attribute '{attr}' for species '{name}' has value "
                f"{getattr(species_dict[name], attr)}, expected {expected_value}."
            )


def validate_full_v1_mechanism(mechanism):
    assert mechanism is not None
    assert mechanism.name == "Full Configuration"
    assert len(mechanism.species) == 11
    validate_species(mechanism.species)
    assert len(mechanism.phases) == 4
    assert len(mechanism.reactions.aqueous_equilibrium) == 1
    assert len(mechanism.reactions.arrhenius) == 2
    assert len(mechanism.reactions.branched) == 1
    assert len(mechanism.reactions.condensed_phase_arrhenius) == 2
    assert len(mechanism.reactions.condensed_phase_photolysis) == 1
    assert len(mechanism.reactions.emission) == 1
    assert len(mechanism.reactions.first_order_loss) == 1
    assert len(mechanism.reactions.henrys_law) == 1
    assert len(mechanism.reactions.photolysis) == 1
    assert len(mechanism.reactions.simpol_phase_transfer) == 1
    assert len(mechanism.reactions.surface) == 1
    assert len(mechanism.reactions.troe) == 1
    assert len(mechanism.reactions.tunneling) == 1
    assert mechanism.version.major == 1
    assert mechanism.version.minor == 0
    assert mechanism.version.patch == 0
    assert len(mechanism.reactions) == 16
    for reaction in mechanism.reactions:
        assert reaction is not None
        assert isinstance(reaction.type, ReactionType)


def test_parsed_full_v1_conofiguration():
    parser = Parser()
    extensions = [".yaml", ".json"]
    for extension in extensions:
        path = f"examples/v1/full_configuration{extension}"
        mechanism = parser.parse(path)
        validate_full_v1_mechanism(mechanism)


def test_parser_reports_bad_files():
    parser = Parser()
    extensions = [".yaml", ".json"]
    for extension in extensions:
        path = f"examples/_missing_configuration{extension}"
        with pytest.raises(Exception):
            parser.parse(path)
