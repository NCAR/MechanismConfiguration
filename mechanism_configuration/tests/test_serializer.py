from mechanism_configuration.utils_constants import SUPPORTED_FILE_EXTENSIONS
from utils_mechanism import mechanism_fully_defined, validate_full_v1_mechanism


def test_serialize():
    # TODO: 
    # use mechanism_fully_defined
    # export a file for each supported file extension
    print()
    for extension in SUPPORTED_FILE_EXTENSIONS:
        path = f"examples/_missing_configuration{extension}"
        print(path)

    # import each file
    # convent import into python object
    # test newly imported object is equal to mechanism_fully_defined
    validate_full_v1_mechanism(mechanism_fully_defined) # change to mechanism from imported files


# TODO: shoud serializer throw errors/exceptions? what errors should it throw?
