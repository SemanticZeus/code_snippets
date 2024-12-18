from rdkit import Chem
from rdkit.Chem import Draw

# Generate a molecule from SMILES
molecule = Chem.MolFromSmiles("CCO")  # Ethanol

# Save an image of the structure
Draw.MolToFile(molecule, "ethanol.png")
