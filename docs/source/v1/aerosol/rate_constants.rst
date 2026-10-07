.. _v1-aerosol-rate-constants:

##############
Rate Constants
##############

Aerosol processes and constraints use these temperature-dependent constants.
Each constant is an object inside the process or constraint.

Arrhenius
=========

.. math::

   k(T) = A \exp\left(\frac{C}{T}\right)

where :math:`T` is the temperature (K).

.. tab-set::

   .. tab-item:: YAML

      .. code-block:: yaml
         :force:

         "rate constant":
           type: ARRHENIUS
           A: 1.0e3
           C: 100.0

   .. tab-item:: JSON

      .. code-block:: json
         :force:

         {
           "rate constant": { "type": "ARRHENIUS", "A": 1.0e3, "C": 100.0 }
         }

.. list-table::
   :header-rows: 1
   :widths: 20 15 65

   * - Key
     - Required
     - Description
   * - ``type``
     - No
     - ``ARRHENIUS``. When ``type`` is not given, the parser uses ``ARRHENIUS``.
   * - ``A``
     - Yes
     - The pre-exponential factor
   * - ``C``
     - Yes
     - The exponential factor (K). This is the negative activation energy divided by the gas constant.

Equilibrium
===========

.. math::

   K(T) = A \exp\left(C \left(\frac{1}{T_0} - \frac{1}{T}\right)\right)

where :math:`T` is the temperature (K) and :math:`T_0` is the reference temperature (K).
:math:`C` is the activation energy divided by the gas constant, and it is positive.

.. tab-set::

   .. tab-item:: YAML

      .. code-block:: yaml
         :force:

         "equilibrium constant":
           type: EQUILIBRIUM
           A: 1.14e-2
           "C [K]": 2300.0
           "T0 [K]": 298.15

   .. tab-item:: JSON

      .. code-block:: json
         :force:

         {
           "equilibrium constant": {
             "type": "EQUILIBRIUM",
             "A": 1.14e-2,
             "C [K]": 2300.0,
             "T0 [K]": 298.15
           }
         }

.. list-table::
   :header-rows: 1
   :widths: 20 15 65

   * - Key
     - Required
     - Description
   * - ``type``
     - No
     - ``EQUILIBRIUM``. In a ``"rate constant"``, ``"forward rate constant"``, or ``"reverse rate constant"``,
       you must give this type. If you do not, the parser reads the object as an Arrhenius constant.
   * - ``A``
     - Yes
     - The value at the reference temperature
   * - ``"C [K]"``
     - Yes
     - The temperature dependence (K)
   * - ``"T0 [K]"``
     - No
     - The reference temperature. The default is 298.15 K.

Henry's Law Constant
====================

.. math::

   H(T) = H_\mathrm{ref} \exp\left(C \left(\frac{1}{T} - \frac{1}{T_0}\right)\right)

where :math:`T` is the temperature (K) and :math:`T_0` is the reference temperature (K).
This equation has the opposite temperature trend to the equilibrium constant.

.. tab-set::

   .. tab-item:: YAML

      .. code-block:: yaml
         :force:

         "Henry's law constant":
           "HLC_ref [mol m-3 Pa-1]": 3.4e-2
           "C [K]": 2400.0
           "T0 [K]": 298.15

   .. tab-item:: JSON

      .. code-block:: json
         :force:

         {
           "Henry's law constant": {
             "HLC_ref [mol m-3 Pa-1]": 3.4e-2,
             "C [K]": 2400.0,
             "T0 [K]": 298.15
           }
         }

.. list-table::
   :header-rows: 1
   :widths: 30 15 55

   * - Key
     - Required
     - Description
   * - ``"HLC_ref [mol m-3 Pa-1]"``
     - Yes
     - The Henry's law constant at the reference temperature
   * - ``"C [K]"``
     - Yes
     - The temperature dependence (K)
   * - ``"T0 [K]"``
     - No
     - The reference temperature. The default is 298.15 K.
