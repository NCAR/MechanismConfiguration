# Copyright (C) 2025 University Corporation for Atmospheric Research
# SPDX-License-Identifier: Apache-2.0
#
# This file is part of the musica Python package.
# For more information, see the LICENSE file in the top-level directory of this distribution.
from typing import Optional, Any, Dict
from _mechanism_configuration._core import _Species, _Phase

class Species(_Species):
    """
    A class representing a species in a chemical mechanism.

    Attributes:
        name (str): The name of the species.
        HLC_298K_mol_m3_Pa (float): Henry's Law Constant at 298K [mol m-3 Pa-1]
        HLC_exponential_factor_K: Henry's Law Constant exponential factor [K]
        diffusion_coefficient_m2_s (float): Diffusion coefficient [m2 s-1]
        N_star (float): A parameter used to calculate the mass accomodation factor (Ervens et al., 2003)
        molecular_weight_kg_mol (float): Molecular weight [kg mol-1]
        density_kg_m3 (float): Density [kg m-3]
        tracer_type (str): The type of tracer ("AEROSOL", "THIRD_BODY", "CONSTANT").
        other_properties (Dict[str, Any]): A dictionary of other properties of the species.
    """

    def __init__(
            self,
            name: Optional[str] = None,
            HLC_298K_mol_m3_Pa: Optional[float] = None,
            HLC_exponential_factor_K: Optional[float] = None,
            diffusion_coefficient_m2_s: Optional[float] = None,
            N_star: Optional[float] = None,
            molecular_weight_kg_mol: Optional[float] = None,
            density_kg_m3: Optional[float] = None,
            tracer_type: Optional[str] = None,
            other_properties: Optional[Dict[str, Any]] = None
    ):
        """
        Initializes the Species object with the given parameters.

        Args:
            name (str): The name of the species.
            HLC_298K_mol_m3_Pa (float): Henry's Law Constant at 298K [mol m-3 Pa-1]
            HLC_exponential_factor_K: Henry's Law Constant exponential factor [K]
            diffusion_coefficient_m2_s (float): Diffusion coefficient [m2 s-1]
            N_star (float): A parameter used to calculate the mass accomodation factor (Ervens et al., 2003)
            molecular_weight_kg_mol (float): Molecular weight [kg mol-1]
            density_kg_m3 (float): Density [kg m-3]
            tracer_type (str): The type of tracer ("AEROSOL", "THIRD_BODY", "CONSTANT").
            other_properties (Dict[str, Any]): A dictionary of other properties of the species.
        """
        super().__init__()
        self.name = name
        self.HLC_298K_mol_m3_Pa = HLC_298K_mol_m3_Pa
        self.HLC_exponential_factor_K = HLC_exponential_factor_K
        self.diffusion_coefficient_m2_s = diffusion_coefficient_m2_s
        self.N_star = N_star
        self.molecular_weight_kg_mol = molecular_weight_kg_mol
        self.density_kg_m3 = density_kg_m3
        self.tracer_type = tracer_type
        self.other_properties = other_properties if other_properties is not None else {}

    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> 'Species':
        """
        Creates a Species object from a dictionary.

        Args:
            data (Dict[str, Any]): A dictionary containing the species data.

        Returns:
            Species: A Species object.
        """
        return _Species.from_dict(data)
    

class Phase(_Phase):
    """
    A class representing a phase in a chemical mechanism.

    Attributes:
        name (str): The name of the phase.
        species (List[Species]): A list of species in the phase.
        other_properties (Dict[str, Any]): A dictionary of other properties of the phase.
    """

    def __init__(
            self, 
            name: Optional[str] = None, 
            species: Optional[list[Species]] = None,
            other_properties: Optional[Dict[str, Any]] = None):
        """
        Initializes the Phase object with the given parameters.

        Args:
            name (str): The name of the phase.
            species (List[Species]): A list of species in the phase.
            other_properties (Dict[str, Any]): A dictionary of other properties of the phase.
        """
        super().__init__()
        self.name = name
        self.species = [s.name for s in species] if species is not None else []
        self.other_properties = other_properties if other_properties is not None else {}


    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> 'Phase':
        """
        Creates a Phase object from a dictionary.

        Args:
            data (Dict[str, Any]): A dictionary containing the phase data.

        Returns:
            Phase: A Phase object.
        """
        return _Phase.from_dict(data)