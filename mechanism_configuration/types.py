# Copyright (C) 2025 University Corporation for Atmospheric Research
# SPDX-License-Identifier: Apache-2.0
#
# This file is part of the musica Python package.
# For more information, see the LICENSE file in the top-level directory of this distribution.
from typing import Optional, Any, Dict
from _mechanism_configuration._core import _Species, _Phase, _ReactionComponent, _Arrhenius, _Troe

BOLTZMANN_CONSTANT_J_K = 1.380649e-23  # J K-1

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
    

class Arrhenius(_Arrhenius):
    """
    A class representing an Arrhenius rate constant.

    k = A * exp( C / T ) * ( T / D )^B * exp( 1 - E * P )
    
    where:
        k = rate constant
        A = pre-exponential factor [(mol m-3)^(n-1)s-1]
        B = temperature exponent [unitless]
        C = exponential term [K-1]
        D = reference temperature [K]
        E = pressure scaling term [Pa-1]
        T = temperature [K]
        P = pressure [Pa]
        n = number of reactants

    Attributes:
        name (str): The name of the Arrhenius rate constant.
        A (float): Pre-exponential factor [(mol m-3)^(n-1)s-1] where n is the number of reactants.
        B (float): Temperature exponent [unitless].
        C (float): Exponential term [K-1].
        D (float): Reference Temperature [K].
        E (float): Pressure scaling term [Pa-1].
        reactants (List[Species | (float, Species)]): A list of reactants involved in the reaction.
        products (List[Species | (float, Species)]): A list of products formed in the reaction.
        gas_phase (Phase): The gas phase in which the reaction occurs.
        other_properties (Dict[str, Any]): A dictionary of other properties of the Arrhenius rate constant.
    """

    def __init__(
            self, 
            name: Optional[str] = None,
            A: Optional[float] = None,
            B: Optional[float] = None,
            C: Optional[float] = None,
            Ea: Optional[float] = None,
            D: Optional[float] = None,
            E: Optional[float] = None,
            reactants: Optional[list[Species | tuple[float, Species]]] = None,
            products: Optional[list[Species | tuple[float, Species]]] = None,
            gas_phase: Optional[Phase] = None,
            other_properties: Optional[Dict[str, Any]] = None
    ):
        """
        Initializes the Arrhenius object with the given parameters.

        Args:
            name (str): The name of the Arrhenius rate constant.
            A (float): Pre-exponential factor [(mol m-3)^(n-1)s-1] where n is the number of reactants.
            B (float): Temperature exponent [unitless].
            C (float): Exponential term [K-1].
            Ea (float): Activation energy [J molecule-1].
            D (float): Reference Temperature [K].
            E (float): Pressure scaling term [Pa-1].
            reactants (List[Species | (float, Species)]): A list of reactants involved in the reaction.
            products (List[Species | (float, Species)]): A list of products formed in the reaction.
            gas_phase (Phase): The gas phase in which the reaction occurs.
            other_properties (Dict[str, Any]): A dictionary of other properties of the Arrhenius rate constant.
        """
        super().__init__()
        self.name = name
        self.A = A
        self.B = B
        if C is not None and Ea is not None:
            raise ValueError("Cannot specify both C and Ea.")
        self.C = -Ea / BOLTZMANN_CONSTANT_J_K if Ea is not None else C
        self.D = D
        self.E = E
        self.reactants = [
            _ReactionComponent(r.name) if isinstance(r, Species) else _ReactionComponent(r[1].name, r[0])
            for r in reactants
        ] if reactants is not None else []
        self.products = [
            _ReactionComponent(p.name) if isinstance(p, Species) else _ReactionComponent(p[1].name, p[0])
            for p in products
        ] if products is not None else []
        self.gas_phase = gas_phase.name if gas_phase is not None else None
        self.other_properties = other_properties if other_properties is not None else {}

    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> 'Arrhenius':
        """
        Creates an Arrhenius object from a dictionary.

        Args:
            data (Dict[str, Any]): A dictionary containing the Arrhenius data.

        Returns:
            Arrhenius: An Arrhenius object.
        """
        return _Arrhenius.from_dict(data)
    

class Troe(_Troe):
    """
    A class representing a Troe rate constant.

    Attributes:
        name (str): The name of the Troe rate constant.
        k0_A (float): Pre-exponential factor for the low-pressure limit [(mol m-3)^(n-1)s-1].
        k0_B (float): Temperature exponent for the low-pressure limit [unitless].
        k0_C (float): Exponential term for the low-pressure limit [K-1].
        kinf_A (float): Pre-exponential factor for the high-pressure limit [(mol m-3)^(n-1)s-1].
        kinf_B (float): Temperature exponent for the high-pressure limit [unitless].
        kinf_C (float): Exponential term for the high-pressure limit [K-1].
        Fc (float): Troe parameter [unitless].
        N (float): Troe parameter [unitless].
        reactants (List[Species | (float, Species)]): A list of reactants involved in the reaction.
        products (List[Species | (float, Species)]): A list of products formed in the reaction.
        gas_phase (Phase): The gas phase in which the reaction occurs.
        other_properties (Dict[str, Any]): A dictionary of other properties of the Troe rate constant.
    """
    def __init__(
            self, 
            name: Optional[str] = None,
            k0_A: Optional[float] = None,
            k0_B: Optional[float] = None,
            k0_C: Optional[float] = None,
            kinf_A: Optional[float] = None,
            kinf_B: Optional[float] = None,
            kinf_C: Optional[float] = None,
            Fc: Optional[float] = None,
            N: Optional[float] = None,
            reactants: Optional[list[Species | tuple[float, Species]]] = None,
            products: Optional[list[Species | tuple[float, Species]]] = None,
            gas_phase: Optional[Phase] = None,
            other_properties: Optional[Dict[str, Any]] = None
    ):
        """
        Initializes the Troe object with the given parameters.

        k0 = k0_A * exp( k0_C / T ) * ( T / 300.0 )^k0_B
        kinf = kinf_A * exp( kinf_C / T ) * ( T / 300.0 )^kinf_B
        k = k0[M] / ( 1 + k0[M] / kinf ) * Fc^(1 + 1/N*(log10(k0[M]/kinf))^2)^-1

        where:
            k = rate constant
            k0 = low-pressure limit rate constant
            kinf = high-pressure limit rate constant
            k0_A = pre-exponential factor for the low-pressure limit [(mol m-3)^(n-1)s-1]
            k0_B = temperature exponent for the low-pressure limit [unitless]
            k0_C = exponential term for the low-pressure limit [K-1]
            kinf_A = pre-exponential factor for the high-pressure limit [(mol m-3)^(n-1)s-1]
            kinf_B = temperature exponent for the high-pressure limit [unitless]
            kinf_C = exponential term for the high-pressure limit [K-1]
            Fc = Troe parameter [unitless]
            N = Troe parameter [unitless]
            T = temperature [K]
            M = concentration of the third body [mol m-3]

        Args:
            name (str): The name of the Troe rate constant.
            k0_A (float): Pre-exponential factor for the low-pressure limit [(mol m-3)^(n-1)s-1].
            k0_B (float): Temperature exponent for the low-pressure limit [unitless].
            k0_C (float): Exponential term for the low-pressure limit [K-1].
            kinf_A (float): Pre-exponential factor for the high-pressure limit [(mol m-3)^(n-1)s-1].
            kinf_B (float): Temperature exponent for the high-pressure limit [unitless].
            kinf_C (float): Exponential term for the high-pressure limit [K-1].
            Fc (float): Troe parameter [unitless].
            N (float): Troe parameter [unitless].
            reactants (List[Species | (float, Species)]): A list of reactants involved in the reaction.
            products (List[Species | (float, Species)]): A list of products formed in the reaction.
            gas_phase (Phase): The gas phase in which the reaction occurs.
            other_properties (Dict[str, Any]): A dictionary of other properties of the Troe rate constant.
        """
        super().__init__()
        self.name = name
        self.k0_A = k0_A
        self.k0_B = k0_B
        self.k0_C = k0_C
        self.kinf_A = kinf_A
        self.kinf_B = kinf_B
        self.kinf_C = kinf_C
        self.Fc = Fc
        self.N = N
        self.reactants = [
            _ReactionComponent(r.name) if isinstance(r, Species) else _ReactionComponent(r[1].name, r[0])
            for r in reactants
        ] if reactants is not None else []
        self.products = [
            _ReactionComponent(p.name) if isinstance(p, Species) else _ReactionComponent(p[1].name, p[0])
            for p in products
        ] if products is not None else []
        self.gas_phase = gas_phase.name if gas_phase is not None else None
        self.other_properties = other_properties if other_properties is not None else {}


    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> 'Troe':
        """
        Creates a Troe object from a dictionary.

        Args:
            data (Dict[str, Any]): A dictionary containing the Troe data.

        Returns:
            Troe: A Troe object.
        """
        return _Troe.from_dict(data)