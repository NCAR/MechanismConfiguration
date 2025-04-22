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
            "unknown_properties": {"__absolute tolerance": "1.0e-10"},
        },
        "ethanol": {
            "diffusion_coefficient": 0.95e-05,
            "n_star": 2.55,
            "molecular_weight": 0.04607,
            "unknown_properties": {"__absolute tolerance": "1.0e-20"},
        },
        "ethanol_aq": {
            "molecular_weight": 0.04607,
            "density": 1000.0,
            "unknown_properties": {"__absolute tolerance": "1.0e-20"},
        },
        "H2O2_aq": {
            "molecular_weight": 0.0340147,
            "density": 1000.0,
            "unknown_properties": {"__absolute tolerance": "1.0e-10"},
        },
        "H2O_aq": {
            "density": 1000.0,
            "molecular_weight": 0.01801,
        },
        "aerosol stuff": {
            "molecular_weight": 0.5,
            "density": 1000.0,
            "unknown_properties": {"__absolute tolerance": "1.0e-20"},
        },
        "more aerosol stuff": {
            "molecular_weight": 0.2,
            "density": 1000.0,
            "unknown_properties": {"__absolute tolerance": "1.0e-20"},
        },
    }

    # Create a dictionary for quick lookup of species by name
    species_dict = {sp.name: sp for sp in species}

    # Validate each expected species
    for name, attributes in expected_species.items():
        assert name in species_dict, f"Species '{name}' is missing."
        for attr, expected_value in attributes.items():
            assert hasattr(
                species_dict[name], attr
            ), f"Attribute '{attr}' is missing for species '{name}'."
            assert getattr(species_dict[name], attr) == expected_value, (
                f"Attribute '{attr}' for species '{name}' has value "
                f"{getattr(species_dict[name], attr)}, expected {expected_value}."
            )


def validate_phases(phases):
    # Define the expected phases and their associated species
    expected_phases = {
        "gas": ["A", "B", "C", "ethanol"],
        "aqueous aerosol": ["H2O2_aq", "H2O_aq", "ethanol_aq", "A", "B", "C"],
        "surface reacting phase": ["aerosol stuff", "more aerosol stuff"],
        "cloud": ["B", "C"],
    }

    # Create a dictionary for quick lookup of phases by name
    phases_dict = {phase.name: phase for phase in phases}

    # Validate each expected phase
    for name, expected_species in expected_phases.items():
        assert name in phases_dict, f"Phase '{name}' is missing."
        assert hasattr(
            phases_dict[name], "species"
        ), f"Phase '{name}' does not have a 'species' attribute."
        phase_species = getattr(phases_dict[name], "species")
        assert set(phase_species) == set(expected_species), (
            f"Phase '{name}' has species {phase_species}, "
            f"expected {expected_species}."
        )


def extract_components(components):
    return [
        {"species name": component.species_name, "coefficient": component.coefficient}
        for component in components
    ]


def validate_arrhenius(reactions):
    assert reactions[0].type == ReactionType.Arrhenius
    assert extract_components(reactions[0].reactants) == [
        {"species name": "B", "coefficient": 1}
    ]
    assert extract_components(reactions[0].products) == [
        {"species name": "C", "coefficient": 1}
    ]
    assert reactions[0].A == 32.1
    assert reactions[0].B == -2.3
    assert reactions[0].C == 102.3
    assert reactions[0].D == 63.4
    assert reactions[0].E == -1.3
    assert reactions[0].name == "my arrhenius"
    assert reactions[0].unknown_properties == {}
    assert reactions[1].type == ReactionType.Arrhenius
    assert extract_components(reactions[1].reactants) == [
        {"species name": "A", "coefficient": 1}
    ]
    assert extract_components(reactions[1].products) == [
        {"species name": "B", "coefficient": 1.2}
    ]
    assert reactions[1].A == 29.3
    assert reactions[1].B == -1.5
    assert reactions[1].C == -101.2 / 1.380649e-23
    assert reactions[1].D == 82.6
    assert reactions[1].E == -0.98
    assert reactions[1].name == "my other arrhenius"
    assert reactions[1].unknown_properties == {}


def validate_henrys_law(reactions):
    assert reactions[0].type == ReactionType.HenrysLaw
    assert reactions[0].gas_phase == "gas"
    assert extract_components([reactions[0].gas_phase_species]) == [
        {"species name": "H2O2", "coefficient": 1}
    ]
    assert reactions[0].aerosol_phase == "aqueous aerosol"
    assert extract_components([reactions[0].aerosol_phase_species]) == [
        {"species name": "H2O2_aq", "coefficient": 1}
    ]
    assert extract_components([reactions[0].aerosol_phase_water]) == [
        {"species name": "H2O_aq", "coefficient": 1}
    ]
    assert reactions[0].name == "my henry's law"


def validate_simpol_phase_transfer(reactions):
    assert reactions[0].type == ReactionType.SimpolPhaseTransfer
    assert reactions[0].gas_phase == "gas"
    assert extract_components([reactions[0].gas_phase_species]) == [
        {"species name": "ethanol", "coefficient": 1}
    ]
    assert reactions[0].aerosol_phase == "aqueous aerosol"
    assert extract_components([reactions[0].aerosol_phase_species]) == [
        {"species name": "ethanol_aq", "coefficient": 1}
    ]
    assert reactions[0].B == [-1.97e03, 2.91e00, 1.96e-03, -4.96e-01]
    assert reactions[0].name == "my simpol"


def validate_aqueous_equilibrium(reactions):
    assert reactions[0].type == ReactionType.AqueousEquilibrium
    assert reactions[0].aerosol_phase == "aqueous aerosol"
    assert reactions[0].aerosol_phase_water == "H2O_aq"
    assert reactions[0].A == 1.14e-2
    assert reactions[0].C == 2300.0
    assert reactions[0].k_reverse == 0.32
    assert extract_components(reactions[0].reactants) == [
        {"species name": "A", "coefficient": 2}
    ]
    assert extract_components(reactions[0].products) == [
        {"species name": "B", "coefficient": 1},
        {"species name": "C", "coefficient": 1},
    ]
    assert reactions[0].name == "my aqueous eq"


def validate_condensed_phase_arrhenius(reactions):
    for reaction in reactions:
        assert reaction.type == ReactionType.CondensedPhaseArrhenius
        assert reaction.aerosol_phase == "aqueous aerosol"
        assert reaction.aerosol_phase_water == "H2O_aq"
        assert reaction.A == 123.45
        assert reaction.B == 1.3
        assert reaction.D == 300.0
        assert reaction.E == 0.6e-5
        assert extract_components(reaction.reactants) == [
            {"species name": "H2O2_aq", "coefficient": 1},
            {"species name": "H2O_aq", "coefficient": 1},
        ]
        assert extract_components(reaction.products) == [
            {"species name": "ethanol_aq", "coefficient": 1}
        ]
        assert reaction.name == "my condensed arrhenius"


def validate_condensed_phase_photolysis(reactions):
    assert reactions[0].type == ReactionType.CondensedPhasePhotolysis
    assert reactions[0].aerosol_phase == "aqueous aerosol"
    assert reactions[0].aerosol_phase_water == "H2O_aq"
    assert extract_components(reactions[0].reactants) == [
        {"species name": "H2O2_aq", "coefficient": 1}
    ]
    assert extract_components(reactions[0].products) == [
        {"species name": "ethanol_aq", "coefficient": 1}
    ]
    assert reactions[0].scaling_factor == 12.3
    assert reactions[0].name == "condensed photo B"


def validate_emission(reactions):
    assert reactions[0].type == ReactionType.Emission
    assert reactions[0].gas_phase == "gas"
    assert extract_components(reactions[0].products) == [
        {"species name": "B", "coefficient": 1}
    ]
    assert reactions[0].scaling_factor == 12.3
    assert reactions[0].name == "my emission"


def validate_first_order_loss(reactions):
    assert reactions[0].type == ReactionType.FirstOrderLoss
    assert reactions[0].gas_phase == "gas"
    assert extract_components(reactions[0].reactants) == [
        {"species name": "C", "coefficient": 1}
    ]
    assert reactions[0].scaling_factor == 12.3
    assert reactions[0].name == "my first order loss"


def validate_photolysis(reactions):
    assert reactions[0].type == ReactionType.Photolysis
    assert reactions[0].gas_phase == "gas"
    assert extract_components(reactions[0].reactants) == [
        {"species name": "B", "coefficient": 1}
    ]
    assert extract_components(reactions[0].products) == [
        {"species name": "C", "coefficient": 1}
    ]
    assert reactions[0].scaling_factor == 12.3
    assert reactions[0].name == "photo B"


def validate_surface(reactions):
    assert reactions[0].type == ReactionType.Surface
    assert reactions[0].gas_phase == "gas"
    assert extract_components([reactions[0].gas_phase_species]) == [
        {"species name": "A", "coefficient": 1}
    ]
    assert reactions[0].reaction_probability == 2.0e-2
    assert extract_components(reactions[0].gas_phase_products) == [
        {"species name": "B", "coefficient": 1},
        {"species name": "C", "coefficient": 1},
    ]
    assert reactions[0].aerosol_phase == "surface reacting phase"
    assert reactions[0].name == "my surface"


def validate_troe(reactions):
    assert reactions[0].type == ReactionType.Troe
    assert reactions[0].gas_phase == "gas"
    assert extract_components(reactions[0].reactants) == [
        {"species name": "B", "coefficient": 1},
        {"species name": "M", "coefficient": 1},
    ]
    assert extract_components(reactions[0].products) == [
        {"species name": "C", "coefficient": 1}
    ]
    assert reactions[0].k0_A == 1.2e-12
    assert reactions[0].k0_B == 167
    assert reactions[0].k0_C == 3
    assert reactions[0].kinf_A == 136
    assert reactions[0].kinf_B == 5
    assert reactions[0].kinf_C == 24
    assert reactions[0].Fc == 0.9
    assert reactions[0].N == 0.8
    assert reactions[0].name == "my troe"


def validate_branched_no_ro2(reactions):
    assert reactions[0].type == ReactionType.Branched
    assert reactions[0].gas_phase == "gas"
    assert extract_components(reactions[0].reactants) == [
        {"species name": "A", "coefficient": 1}
    ]
    assert extract_components(reactions[0].alkoxy_products) == [
        {"species name": "B", "coefficient": 1}
    ]
    assert extract_components(reactions[0].nitrate_products) == [
        {"species name": "C", "coefficient": 1}
    ]
    assert reactions[0].X == 1.2e-4
    assert reactions[0].Y == 167
    assert reactions[0].a0 == 0.15
    assert reactions[0].n == 9
    assert reactions[0].name == "my branched"


def validate_tunneling(reactions):
    assert reactions[0].type == ReactionType.Tunneling
    assert reactions[0].gas_phase == "gas"
    assert extract_components(reactions[0].reactants) == [
        {"species name": "B", "coefficient": 1}
    ]
    assert extract_components(reactions[0].products) == [
        {"species name": "C", "coefficient": 1}
    ]
    assert reactions[0].A == 123.45
    assert reactions[0].B == 1200.0
    assert reactions[0].C == 1.0e8
    assert reactions[0].name == "my tunneling"


def validate_full_v1_mechanism(mechanism):
    assert mechanism is not None
    assert mechanism.name == "Full Configuration"
    assert len(mechanism.species) == 11
    validate_species(mechanism.species)
    assert len(mechanism.phases) == 4
    validate_phases(mechanism.phases)
    assert len(mechanism.reactions.aqueous_equilibrium) == 1
    validate_aqueous_equilibrium(mechanism.reactions.aqueous_equilibrium)
    assert len(mechanism.reactions.arrhenius) == 2
    validate_arrhenius(mechanism.reactions.arrhenius)
    assert len(mechanism.reactions.branched) == 1
    validate_branched_no_ro2(mechanism.reactions.branched)
    assert len(mechanism.reactions.condensed_phase_arrhenius) == 2
    validate_condensed_phase_arrhenius(mechanism.reactions.condensed_phase_arrhenius)
    assert len(mechanism.reactions.condensed_phase_photolysis) == 1
    validate_condensed_phase_photolysis(
        mechanism.reactions.condensed_phase_photolysis
    )
    assert len(mechanism.reactions.emission) == 1
    validate_emission(mechanism.reactions.emission)
    assert len(mechanism.reactions.first_order_loss) == 1
    validate_first_order_loss(mechanism.reactions.first_order_loss)
    assert len(mechanism.reactions.henrys_law) == 1
    validate_henrys_law(mechanism.reactions.henrys_law)
    assert len(mechanism.reactions.photolysis) == 1
    validate_photolysis(mechanism.reactions.photolysis)
    assert len(mechanism.reactions.simpol_phase_transfer) == 1
    validate_simpol_phase_transfer(mechanism.reactions.simpol_phase_transfer)
    assert len(mechanism.reactions.surface) == 1
    validate_surface(mechanism.reactions.surface)
    assert len(mechanism.reactions.troe) == 1
    validate_troe(mechanism.reactions.troe)
    assert len(mechanism.reactions.tunneling) == 1
    validate_tunneling(mechanism.reactions.tunneling)
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
