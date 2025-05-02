import pytest
from mechanism_configuration import *
from mechanism_configuration.utils_constants import SUPPORTED_FILE_EXTENSIONS
from utils_mechanism import mechanism_fully_defined, validate_full_v1_mechanism


def test_parsed_full_v1_configuration():
    parser = Parser()
    for extension in SUPPORTED_FILE_EXTENSIONS:
        path = f"examples/v1/full_configuration{extension}"
        mechanism = parser.parse(path)
        validate_full_v1_mechanism(mechanism)


def test_parser_reports_bad_files():
    parser = Parser()
    for extension in SUPPORTED_FILE_EXTENSIONS:
        path = f"examples/_missing_configuration{extension}"
        with pytest.raises(Exception):
            parser.parse(path)


def test_hard_coded_full_v1_configuration():
    validate_full_v1_mechanism(mechanism_fully_defined)


def test_hard_coded_default_constructed_types():
    arrhenius = Arrhenius()
    assert arrhenius.type == ReactionType.Arrhenius
    condensed_phase_arrhenius = CondensedPhaseArrhenius()
    assert condensed_phase_arrhenius.type == ReactionType.CondensedPhaseArrhenius
    condensed_phase_photolysis = CondensedPhasePhotolysis()
    assert condensed_phase_photolysis.type == ReactionType.CondensedPhasePhotolysis
    emission = Emission()
    assert emission.type == ReactionType.Emission
    first_order_loss = FirstOrderLoss()
    assert first_order_loss.type == ReactionType.FirstOrderLoss
    henrys_law = HenrysLaw()
    assert henrys_law.type == ReactionType.HenrysLaw
    photolysis = Photolysis()
    assert photolysis.type == ReactionType.Photolysis
    simpol_phase_transfer = SimpolPhaseTransfer()
    assert simpol_phase_transfer.type == ReactionType.SimpolPhaseTransfer
    surface = Surface()
    assert surface.type == ReactionType.Surface
    troe = Troe()
    assert troe.type == ReactionType.Troe
    tunneling = Tunneling()
    assert tunneling.type == ReactionType.Tunneling
    wet_deposition = WetDeposition()
    assert wet_deposition.type == ReactionType.WetDeposition
    branched = Branched()
    assert branched.type == ReactionType.Branched
    user_defined = UserDefined()
    assert user_defined.type == ReactionType.UserDefined
